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
template<> smart_ptr<Bmark_descr> OriginalBenchmark<BenchmarkSuite<BS_MPI1>, BMRK_FN>::descr = NULL; \
DECLARE_INHERITED_TEMPLATE(GLUE_TYPENAME(OriginalBenchmark<BenchmarkSuite<BS_MPI1>, BMRK_FN>), BMRK_NAME) \
template class OriginalBenchmark<BenchmarkSuite<BS_MPI1>, BMRK_FN>; \
template<> bool OriginalBenchmark<BenchmarkSuite<BS_MPI1>, BMRK_FN>::init_description() 


BENCHMARK(IMB_pingpong, PingPong)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SELECT_SOURCE);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(SCALE_TIME_HALF);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
//    descr->comments.push_back("This is a pingpong benchmark bla bla bla...");
    return true;
}

void IMB_pingpong_specificsource(struct comm_info* c_info, int size,
                                 struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_pingpong(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_pingpong_specificsource, PingPongSpecificSource)
{
    descr->flags.insert(SELECT_SOURCE);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(SCALE_TIME_HALF);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    return true;
}

void IMB_pingpong_anysource(struct comm_info* c_info, int size,
                            struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_pingpong(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_pingpong_anysource, PingPongAnySource)
{
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(SCALE_TIME_HALF);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    return true;
}

BENCHMARK(IMB_pingping, PingPing)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SELECT_SOURCE);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    return true;
}

void IMB_pingping_specificsource(struct comm_info* c_info, int size,
                                 struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_pingping(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_pingping_specificsource, PingPingSpecificSource)
{
    descr->flags.insert(SELECT_SOURCE);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    return true;
}

void IMB_pingping_anysource(struct comm_info* c_info, int size,
                            struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_pingping(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_pingping_anysource, PingPingAnySource)
{
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    return true;
}

BENCHMARK(IMB_sendrecv, Sendrecv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(PARALLEL_TRANSFER);
    descr->flags.insert(SCALE_BW_DOUBLE);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    return true;
}

BENCHMARK(IMB_exchange, Exchange)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(PARALLEL_TRANSFER);
    descr->flags.insert(SCALE_BW_FOUR);
    descr->flags.insert(SENDBUF_SIZE_2I);
    descr->flags.insert(RECVBUF_SIZE_I);
    return true;
}

BENCHMARK(IMB_allreduce, Allreduce)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    return true;
}

BENCHMARK(IMB_reduce, Reduce)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    return true;
}

BENCHMARK(IMB_reduce_local, Reduce_local)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    return true;
}

BENCHMARK(IMB_reduce_scatter, Reduce_scatter)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    return true;
}

BENCHMARK(IMB_reduce_scatter_block, Reduce_scatter_block)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    return true;
}

BENCHMARK(IMB_allgather, Allgather)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    return true;
}

BENCHMARK(IMB_allgatherv, Allgatherv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    return true;
}

BENCHMARK(IMB_gather, Gather)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(HAS_ROOT);
    return true;
}

BENCHMARK(IMB_gatherv, Gatherv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(HAS_ROOT);
    return true;
}

BENCHMARK(IMB_scatter, Scatter)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(HAS_ROOT);
    return true;
}

BENCHMARK(IMB_scatterv, Scatterv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(HAS_ROOT);
    return true;
}

BENCHMARK(IMB_alltoall, Alltoall)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    return true;
}

BENCHMARK(IMB_alltoallv, Alltoallv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    return true;
}

BENCHMARK(IMB_bcast, Bcast)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    return true;
}

BENCHMARK(IMB_barrier, Barrier)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SYNC);
    descr->flags.insert(SENDBUF_SIZE_0);
    descr->flags.insert(RECVBUF_SIZE_0);
    return true;
}

BENCHMARK(IMB_uni_bandwidth, Uniband)
{
    descr->flags.insert(PARALLEL_TRANSFER_MSG_RATE);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    return true;
}

BENCHMARK(IMB_bi_bandwidth, Biband)
{
    descr->flags.insert(PARALLEL_TRANSFER_MSG_RATE);
    descr->flags.insert(SCALE_BW_DOUBLE);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    return true;
}
