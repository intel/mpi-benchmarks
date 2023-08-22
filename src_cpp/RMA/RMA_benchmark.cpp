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

#define BENCHMARK(BMRK_FN, BMRK_NAME) \
template<> smart_ptr<Bmark_descr> OriginalBenchmark<BenchmarkSuite<BS_RMA>, BMRK_FN>::descr = NULL; \
DECLARE_INHERITED_TEMPLATE(GLUE_TYPENAME(OriginalBenchmark<BenchmarkSuite<BS_RMA>, BMRK_FN>), BMRK_NAME) \
template class OriginalBenchmark<BenchmarkSuite<BS_RMA>, BMRK_FN>; \
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
