/****************************************************************************
*                                                                           *
* Copyright (C) 2024 Intel Corporation                                      *
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
#include <stdio.h>
#include "benchmark.h"
#include "args_parser.h"
#include "utils.h"
#include "benchmark_suites_collection.h"
#include "benchmark_suite.h"
#include "utils.h"

#include "MT_types.h"

namespace NS_MT {
    std::vector<thread_local_data_t> input;
    int mode_multiple;
    int stride;
    int num_threads;
    int window_size;
    int rank;
    bool prepared = false;
    std::vector<int> count;
    int malloc_align;
    malopt_t malloc_option;
    barropt_t barrier_option;
    bool do_checks;
    MPI_Datatype datatype;
    bool noheader;
}


DECLARE_BENCHMARK_SUITE_STUFF(BS_MT, IMB-MT)

template <> bool BenchmarkSuite<BS_MT>::declare_args(args_parser &parser,
                                                     std::ostream &output) const {
    UNUSED(output);
    parser.set_current_group(get_name());
    parser.add<int>("stride", 0);
    parser.add<int>("warmup",  100);
    parser.add<int>("repeat", 1000);
    parser.add<int>("window_size", 64);
    parser.add<std::string>("barrier", "on").set_caption("on|off|special");
    parser.add_vector<int>("count", "1,2,4,8").
        set_mode(args_parser::option::APPLY_DEFAULTS_ONLY_WHEN_MISSING);
    parser.add<int>("malloc_align", 64);
    parser.add<std::string>("malloc_algo", "serial").set_caption("serial|continuous|parallel");
    parser.add<bool>("check", false);
    parser.add_flag("noheader");
    parser.add<std::string>("datatype", "int").set_caption("int|char");
    parser.set_default_current_group();
    return true;
}

template <> bool BenchmarkSuite<BS_MT>::prepare(const args_parser &parser, 
                                                const std::vector<std::string> &benchs,
                                                const std::vector<std::string> &unknown_args,
                                                std::ostream &output) {
    using namespace NS_MT;

    if (unknown_args.size() != 0) {
        std::vector<std::string> unknown_options, unknown_benchmarks;
        for (std::vector<std::string>::const_iterator it = unknown_args.begin(); it != unknown_args.end(); ++it) {
            if (parser.is_option(*it)) {
                unknown_options.push_back(*it);
            }
            else {
                unknown_benchmarks.push_back(*it);
            }
        }
        for (std::vector<std::string>::const_iterator it = unknown_options.begin(); it != unknown_options.end(); ++it) {
            output << "Invalid option " << *it << std::endl;
        }
        for (std::vector<std::string>::const_iterator it = unknown_benchmarks.begin(); it != unknown_benchmarks.end(); ++it) {
            output << "Invalid benchmark name " << *it << std::endl;
        }
        return false;
    }
    std::vector<std::string> all_benchs, spare_benchs = benchs, intersection = benchs;
    BenchmarkSuite<BS_MT>::get_full_list(all_benchs);
    set_operations::exclude(spare_benchs, all_benchs);
    set_operations::exclude(intersection, spare_benchs);
    if (intersection.size() == 0)
        return true;

    parser.get<int>("count", count);
    mode_multiple = (parser.get<std::string>("thread_level") == "multiple");
    stride = parser.get<int>("stride");
    
    std::string barrier_type = parser.get<std::string>("barrier");
    if (barrier_type == "off") barrier_option = BARROPT_NOBARRIER;
    else if (barrier_type == "on") barrier_option = BARROPT_NORMAL;
    else if (barrier_type == "special") barrier_option = BARROPT_SPECIAL;
    else {
        output << get_name() << ": " << "Wrong barrier option value" << std::endl;
        return false;
    }

    malloc_align = parser.get<int>("malloc_align");

    std::string malloc_algo = parser.get<std::string>("malloc_algo");
    if (malloc_algo == "serial") malloc_option = MALOPT_SERIAL;
    else if (malloc_algo == "continuous") malloc_option = MALOPT_CONTINUOUS;
    else if (malloc_algo == "parallel") malloc_option = MALOPT_PARALLEL;
    else {
        output << get_name() << ": " << "Wrong malloc_algo option value" << std::endl;
        return false;
    }
    if ((malloc_option == MALOPT_PARALLEL || malloc_option == MALOPT_CONTINUOUS) && !mode_multiple) {
        malloc_option = MALOPT_SERIAL;
    }

    do_checks = parser.get<bool>("check");

    noheader = parser.get<bool>("noheader");

    std::string dt = parser.get<std::string>("datatype");
    if (dt == "int") datatype = MPI_INT;
    else if (dt == "char") datatype = MPI_CHAR;
    else {
        output << get_name() << ": " << "Unknown data type in datatype option" << std::endl;
        return false;
    }

    if (do_checks && datatype != MPI_INT) {
        output << get_name() << ": " << "Only int data type is supported with check option" << std::endl;
        return false;
    }

    num_threads = 1;
    if (mode_multiple) {
#pragma omp parallel default(shared)
#pragma omp master
        num_threads = omp_get_num_threads();
    }
    input.resize(num_threads);
    window_size = parser.get<int>("window_size");
    for (int thread_num = 0; thread_num < num_threads; thread_num++) {
        input[thread_num].comm = duplicate_comm(mode_multiple, thread_num);
        input[thread_num].warmup = parser.get<int>("warmup");
        input[thread_num].repeat = parser.get<int>("repeat");
    }
    prepared = true;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0 && !noheader) {
        output << "#------------------------------------------------------------------" << std::endl;
        output << "#    Intel(R) MPI Benchmarks " << "2021.8" << ", MT part    " << std::endl;
        output << "#------------------------------------------------------------------" << std::endl;
        output << "#" << std::endl;
    }
    return true;
}

template <> void BenchmarkSuite<BS_MT>::finalize(const std::vector<std::string> &,
                                                 std::ostream &output) {
    using namespace NS_MT;
    if (prepared && rank == 0)
        output << std::endl;
}

#define HANDLE_PARAMETER(TYPE, NAME) if (key == #NAME) { \
                                        result = smart_ptr< TYPE >(&NAME); \
                                        result.detach_ptr(); }

template <> any BenchmarkSuite<BS_MT>::get_parameter(const std::string &key)
{
    using namespace NS_MT;
    any result;
    HANDLE_PARAMETER(std::vector<thread_local_data_t>, input);
    HANDLE_PARAMETER(int, num_threads);
    HANDLE_PARAMETER(int, window_size);
    HANDLE_PARAMETER(int, mode_multiple);
    HANDLE_PARAMETER(int, stride);
    HANDLE_PARAMETER(int, malloc_align);
    HANDLE_PARAMETER(malopt_t, malloc_option);
    HANDLE_PARAMETER(barropt_t, barrier_option);
    HANDLE_PARAMETER(bool, do_checks);
    HANDLE_PARAMETER(MPI_Datatype, datatype);
    HANDLE_PARAMETER(std::vector<int>, count);
    return result;
}

#ifdef WIN32
template BenchmarkSuite<BS_MT>;
#endif
