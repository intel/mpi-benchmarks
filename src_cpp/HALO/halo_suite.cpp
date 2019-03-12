/*****************************************************************************
 *                                                                           *
 * Copyright 2016-2019 Intel Corporation.                                    *
 *                                                                           *
 *****************************************************************************

This code is covered by the Community Source License (CPL), version
1.0 as published by IBM and reproduced in the file "license.txt" in the
"license" subdirectory. Redistribution in source and binary form, with
or without modification, is permitted ONLY within the regulations
contained in above mentioned license.

Use of the name and trademark "Intel(R) MPI Benchmarks" is allowed ONLY
within the regulations of the "License for Use of "Intel(R) MPI
Benchmarks" Name and Trademark" as reproduced in the file
"use-of-trademark-license.txt" in the "license" subdirectory.

THE PROGRAM IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT
LIMITATION, ANY WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT,
MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Each Recipient is
solely responsible for determining the appropriateness of using and
distributing the Program and assumes all risks associated with its
exercise of rights under this Agreement, including but not limited to
the risks and costs of program errors, compliance with applicable
laws, damage to or loss of data, programs or equipment, and
unavailability or interruption of operations.

EXCEPT AS EXPRESSLY SET FORTH IN THIS AGREEMENT, NEITHER RECIPIENT NOR
ANY CONTRIBUTORS SHALL HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING
WITHOUT LIMITATION LOST PROFITS), HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OR
DISTRIBUTION OF THE PROGRAM OR THE EXERCISE OF ANY RIGHTS GRANTED
HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF
YOUR JURISDICTION. It is licensee's responsibility to comply with any
export regulations applicable in licensee's jurisdiction. Under
CURRENT U.S. export regulations this software is eligible for export
from the U.S. and can be downloaded by or otherwise exported or
reexported worldwide EXCEPT to U.S. embargoed destinations which
include Cuba, Iraq, Libya, North Korea, Iran, Syria, Sudan,
Afghanistan and any other country to which the U.S. has embargoed
goods and services.

 ***************************************************************************
*/

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

