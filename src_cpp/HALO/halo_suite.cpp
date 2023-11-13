/****************************************************************************
*                                                                           *
* Copyright (C) 2023 Intel Corporation                                      *
*                                                                           *
*****************************************************************************

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ***************************************************************************/

#include <mpi.h>
#include <omp.h>
#include <set>
#include <vector>
#include <string>
#include <map>
#include <math.h>
#include <stdio.h>
#include "utils.h"
#include "benchmark.h"
#include "benchmark_suites_collection.h"
#include "args_parser.h"

namespace ndim_halo_benchmark {

#include "benchmark_suite.h"


template <typename integer>
integer gcd(integer a, integer b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    if (a == 0) return b;
    while (b != 0) {
        integer remainder = a % b;
        a = b;
        b = remainder;
    }
    return a;
}

#include "MT_types.h"

namespace NS_HALO {
    std::vector<thread_local_data_t> input;
    int mode_multiple;
    int stride;
    int num_threads;
    int rank, nranks;
    bool prepared = false;
    std::vector<int> count;
    int malloc_align;
    bool do_checks;
    MPI_Datatype datatype;
    int required_nranks, ndims;
    std::vector<int> ranksperdim;
    std::vector<int> mults;
    std::vector<unsigned int> mysubs;
    static void fill_in(std::vector<int> &topo)
    {
        ndims = topo.size();
        ranksperdim = topo;
        {
            int n = 0;
            for (int i = 0; i < ndims; ++i) {
                n = gcd(n, topo[i]);
            }
            assert(n > 0);
            for (int i = 0; i < ndims; ++i) {
                ranksperdim[i] = topo[i] / n;
            }
        }
        required_nranks = 1;
        for (int i = 0; i < ndims; ++i)
            required_nranks *= ranksperdim[i];
        if (nranks / required_nranks >= (1<<ndims)) {
            int mult = (int)(pow(nranks, 1.0/ndims));
            for (int i = 0; i < ndims; ++i)
                ranksperdim[i] *= mult;
            required_nranks = 1;
            for (int i = 0; i < ndims; ++i)
                required_nranks *= ranksperdim[i];
        }
        mults.resize(ndims);
        mults[ndims - 1] = 1;
        for (int i = ndims - 2; i >= 0; --i)
            mults[i] = mults[i + 1] * ranksperdim[i + 1];
    }
}

DECLARE_BENCHMARK_SUITE_STUFF(BS_GENERIC, ndim_halo_benchmark)

template <> bool BenchmarkSuite<BS_GENERIC>::declare_args(args_parser &parser,
                                                          std::ostream &output) const {
    UNUSED(output);
    parser.set_current_group(get_name());
    parser.add<int>("stride", 0);
    parser.add<int>("warmup",  100);
    parser.add<int>("repeat", 1000);
    parser.add_vector<int>("count", "1,2,4,8").
        set_mode(args_parser::option::APPLY_DEFAULTS_ONLY_WHEN_MISSING);
    parser.add<int>("malloc_align", 64);
    parser.add<std::string>("datatype", "int").
        set_caption("int|char");
    parser.add_vector<int>("topo", "1", '.');
    parser.set_default_current_group();
    return true;
}

template <> bool BenchmarkSuite<BS_GENERIC>::prepare(const args_parser &parser, 
                                                     const std::vector<std::string> &,
                                                     const std::vector<std::string> &unknown_args,
                                                     std::ostream &output) {
    if (unknown_args.size() != 0) {
        output << "Some unknown options or extra arguments." << std::endl;
        return false;
    }
    using namespace NS_HALO;

    parser.get<int>("count", count);
    mode_multiple = (parser.get<std::string>("thread_level") == "multiple");
    stride = parser.get<int>("stride");

    malloc_align = parser.get<int>("malloc_align");


    std::string dt = parser.get<std::string>("datatype");
    if (dt == "int") datatype = MPI_INT;
    else if (dt == "char") datatype = MPI_CHAR;
    else {
        output << get_name() << ": " << "Unknown data type in datatype option" << std::endl;
        return false;
    }

    num_threads = 1;
    if (mode_multiple) {
#pragma omp parallel default(shared)
#pragma omp master
        num_threads = omp_get_num_threads();
    }
    input.resize(num_threads);    
    for (int thread_num = 0; thread_num < num_threads; thread_num++) {
        input[thread_num].comm = duplicate_comm(mode_multiple, thread_num);
        input[thread_num].warmup = parser.get<int>("warmup");
        input[thread_num].repeat = parser.get<int>("repeat");
    }

    std::vector<int> topo;
    parser.get<int>("topo", topo);

    // -- HALO specific part
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    fill_in(topo);

    if (required_nranks > nranks && rank == 0) {
        output << get_name() << ": " << "Not enough ranks, " << required_nranks << " min. required" << std::endl;
        return false;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    prepared = true;
    return true;
}

#define HANDLE_PARAMETER(TYPE, NAME) if (key == #NAME) { \
                                        result = smart_ptr< TYPE >(&NAME); \
                                        result.detach_ptr(); }


template<> any BenchmarkSuite<BS_GENERIC>::get_parameter(const std::string &key) {
    using namespace NS_HALO;
    assert(prepared);
    any result;
    HANDLE_PARAMETER(std::vector<thread_local_data_t>, input);
    HANDLE_PARAMETER(int, num_threads);   
    HANDLE_PARAMETER(int, mode_multiple);
    HANDLE_PARAMETER(int, malloc_align);
    HANDLE_PARAMETER(MPI_Datatype, datatype);
    HANDLE_PARAMETER(int, ndims);
    HANDLE_PARAMETER(int, required_nranks);
    HANDLE_PARAMETER(std::vector<int>, ranksperdim);
    HANDLE_PARAMETER(std::vector<int>, mults);
    HANDLE_PARAMETER(int, rank);
    HANDLE_PARAMETER(int, nranks);
    HANDLE_PARAMETER(std::vector<int>, count);
    return result;
}

}

