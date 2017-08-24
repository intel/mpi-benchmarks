/*****************************************************************************
 *                                                                           *
 * Copyright (c) 2016-2017 Intel Corporation.                                *
 * All rights reserved.                                                      *
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
    int rank;
    bool prepared = false;
    std::vector<int> count;
    int malloc_align;
    malopt_t malloc_option;
    barropt_t barrier_option;
    bool do_checks;
    MPI_Datatype datatype;
}


DECLARE_BENCHMARK_SUITE_STUFF(BS_MT, IMB-MT)

template <> void BenchmarkSuite<BS_MT>::declare_args(args_parser &parser) const {
    parser.add<int>("stride", 0);
    parser.add<int>("warmup",  100);
    parser.add<int>("repeat", 1000);
    parser.add<std::string>("barrier", "on").set_caption("on|off|special");
    parser.add_vector<int>("count", "1,2,4,8").
        set_mode(args_parser::option::APPLY_DEFAULTS_ONLY_WHEN_MISSING);
    parser.add<int>("malloc_align", 64);
    parser.add<std::string>("malloc_algo", "serial").set_caption("serial|continous|parallel");
    parser.add<bool>("check", false);
    parser.add<std::string>("datatype", "int").set_caption("int|char");
}

template <> bool BenchmarkSuite<BS_MT>::prepare(const args_parser &parser, const std::set<std::string> &) {
    using namespace NS_MT;
    parser.get<int>("count", count);
    mode_multiple = (parser.get<std::string>("thread_level") == "multiple");
    stride = parser.get<int>("stride");
    
    std::string barrier_type = parser.get<std::string>("barrier");
    if (barrier_type == "off") barrier_option = BARROPT_NOBARRIER;
    else if (barrier_type == "on") barrier_option = BARROPT_NORMAL;
    else if (barrier_type == "special") barrier_option = BARROPT_SPECIAL;
    else {
        // FIXME get rid of cout some way!
        std::cout << "Wrong barrier option value" << std::endl;
        return false;
    }

    malloc_align = parser.get<int>("malloc_align");

    std::string malloc_algo = parser.get<std::string>("malloc_algo");
    if (malloc_algo == "serial") malloc_option = MALOPT_SERIAL;
    else if (malloc_algo == "continous") malloc_option = MALOPT_CONTINOUS;
    else if (malloc_algo == "parallel") malloc_option = MALOPT_PARALLEL;
    else {
        // FIXME get rid of cout some way!
        std::cout << "Wrong malloc_algo option value" << std::endl;
        return false;
    }
    if ((malloc_option == MALOPT_PARALLEL || malloc_option == MALOPT_CONTINOUS) && !mode_multiple) {
        malloc_option = MALOPT_SERIAL;
    }

    do_checks = parser.get<bool>("check");

    std::string dt = parser.get<std::string>("datatype");
    if (dt == "int") datatype = MPI_INT;
    else if (dt == "char") datatype = MPI_CHAR;
    else {
        // FIXME get rid of cout some way!
        std::cout << "Unknown data type in datatype option" << std::endl;
        return false;
    }

    if (do_checks && datatype != MPI_INT) {
        // FIXME get rid of cout some way!
        std::cout << "Only int data type is supported with check option" << std::endl;
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
    prepared = true;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        std::cout << "#------------------------------------------------------------" << std::endl;
        std::cout << "#    Intel(R) MPI Benchmarks " << "PREVIEW" << ", MT part    " << std::endl;
        std::cout << "#------------------------------------------------------------" << std::endl;
        std::cout << "#" << std::endl;
        std::cout << "# ******* WARNING! THIS IS PREVIEW VERSION!      *******" << std::endl;
        std::cout << "# ******* FOR PRELIMINARY OVERVIEW ONLY!         *******" << std::endl;
        std::cout << "# ******* DON'T USE FOR ANY ACTUAL BENCHMARKING! *******" << std::endl;
        std::cout << "#" << std::endl;
        std::cout << "#" << std::endl;
    }
    return true;
}

template <> void BenchmarkSuite<BS_MT>::finalize(const std::set<std::string> &) {
    using namespace NS_MT;
    if (prepared && rank == 0)
        std::cout << std::endl;
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

