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
#include <vector>
#include <string>
#include <map>
#include <set>
#include <iostream>
#include "smart_ptr.h"
#include "benchmark_suites_collection.h"
#include "utils.h"
#include "benchmark_suite.h"
#include "original_benchmark.h"

extern "C" {
#include "IMB_benchmark.h"
#include "IMB_comm_info.h"
#include "IMB_prototypes.h"
}

#include "helper_IMB_functions.h"

using namespace std;

#define BENCHMARK(BMRK_FN, BMRK_NAME) template class OriginalBenchmark<BenchmarkSuite<BS_RMA>, BMRK_FN>; \
DECLARE_INHERITED_TEMPLATE(GLUE_TYPENAME(OriginalBenchmark<BenchmarkSuite<BS_RMA>, BMRK_FN>), BMRK_NAME) \
template<> smart_ptr<Bmark_descr> OriginalBenchmark<BenchmarkSuite<BS_RMA>, BMRK_FN>::descr = NULL; \
template<> bool OriginalBenchmark<BenchmarkSuite<BS_RMA>, BMRK_FN>::init_description() 


BENCHMARK(IMB_rma_single_put, Unidir_put)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(PUT);
    return true;
}

void IMB_rma_Bidir_put(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_rma_single_put(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_rma_Bidir_put, Bidir_put)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(BIDIR_1);
    descr->flags.insert(PUT);
    return true;
}

BENCHMARK(IMB_rma_single_get, Unidir_get)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(GET);
    return true;
}

void IMB_rma_Bidir_get(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_rma_single_get(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_rma_Bidir_get, Bidir_get)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(BIDIR_1);
    descr->flags.insert(GET);
    return true;
}

BENCHMARK(IMB_rma_put_local, Put_local)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(PUT);
    return true;
}

BENCHMARK(IMB_rma_get_local, Get_local)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(GET);
    return true;
}

BENCHMARK(IMB_rma_put_all_local, Put_all_local)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(MULT_PASSIVE_TRANSFER);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(NON_AGGREGATE);
    descr->flags.insert(PUT);
    return true;
}

BENCHMARK(IMB_rma_get_all_local, Get_all_local)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(MULT_PASSIVE_TRANSFER);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(NON_AGGREGATE);
    descr->flags.insert(GET);
    return true;
}

BENCHMARK(IMB_rma_put_all, One_put_all)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(MULT_PASSIVE_TRANSFER);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(NON_AGGREGATE);
    descr->flags.insert(PUT);
    return true;
}

void IMB_rma_All_put_all(struct comm_info* c_info, int size,
                         struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_rma_put_all(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_rma_All_put_all, All_put_all)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(NON_AGGREGATE);
    descr->flags.insert(PUT);
    return true;
}

BENCHMARK(IMB_rma_get_all, One_get_all)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(MULT_PASSIVE_TRANSFER);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(NON_AGGREGATE);
    descr->flags.insert(GET);
    return true;
}

void IMB_rma_All_get_all(struct comm_info* c_info, int size,
                         struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_rma_get_all(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_rma_All_get_all, All_get_all)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(NON_AGGREGATE);
    descr->flags.insert(GET);
    return true;
}

BENCHMARK(IMB_rma_exchange_put, Exchange_put)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_2I);
    descr->flags.insert(RECVBUF_SIZE_2I);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(NON_AGGREGATE);
    descr->flags.insert(PUT);
    return true;
}

BENCHMARK(IMB_rma_exchange_get, Exchange_get)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_2I);
    descr->flags.insert(RECVBUF_SIZE_2I);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(NON_AGGREGATE);
    descr->flags.insert(GET);
    return true;
}

BENCHMARK(IMB_rma_accumulate, Accumulate)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(PUT);
    return true;
}

BENCHMARK(IMB_rma_get_accumulate, Get_accumulate)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(PUT);
    return true;
}

BENCHMARK(IMB_rma_fetch_and_op, Fetch_and_op)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_ELEMENT_TRANSFER);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(PUT);
    return true;
}

BENCHMARK(IMB_rma_compare_and_swap, Compare_and_swap)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_3I);
    descr->flags.insert(SINGLE_ELEMENT_TRANSFER);
    descr->flags.insert(PUT);
    descr->comments.push_back("Uses MPI_INT data type");
    return true;
}

BENCHMARK(IMB_rma_passive_put, Truly_passive_put)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(NON_AGGREGATE);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(PUT);
    descr->comments.push_back("The benchmark measures execution time of MPI_Put for 2 cases:");
    descr->comments.push_back("1) The target is waiting in MPI_Barrier call (t_pure value)");
    descr->comments.push_back("2) The target performs computation and then enters MPI_Barrier routine (t_ovrl value)");
    return true;
}
