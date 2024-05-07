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
template<> BenchmarkSuite<BS_MPI4>::pnames_t *BenchmarkSuite<BS_MPI4>::pnames; \
template<> BenchmarkSuite<BS_MPI4> *BenchmarkSuite<BS_MPI4>::instance; \
template<> smart_ptr<Bmark_descr> OriginalBenchmark<BenchmarkSuite<BS_MPI4>, BMRK_FN>::descr = NULL; \
DECLARE_INHERITED_TEMPLATE(GLUE_TYPENAME(OriginalBenchmark<BenchmarkSuite<BS_MPI4>, BMRK_FN>), BMRK_NAME) \
template class OriginalBenchmark<BenchmarkSuite<BS_MPI4>, BMRK_FN>; \
template<> bool OriginalBenchmark<BenchmarkSuite<BS_MPI4>, BMRK_FN>::init_description() 

BENCHMARK(IMB_bcast_persist, Bcast_persist)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_bcast_pure_persist, Bcast_pure_persist)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(COLLECTIVE);
    return true;
}
/*
BENCHMARK(IMB_allgather_persist, Allgather_persist)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_allgather_pure_persist, Allgather_pure_persist)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_allgatherv_persist, Allgatherv_persist)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_allgatherv_pure_persist, Allgatherv_pure_persist)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_gather_persist, Gather_persist)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_gather_pure_persist, Gather_pure_persist)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_gatherv_persist, Gatherv_persist)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_gatherv_pure_persist, Gatherv_pure_persist)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(COLLECTIVE);
    return true;
} */

BENCHMARK(IMB_alltoall_persist, Alltoall_persist)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_alltoall_pure_persist, Alltoall_pure_persist)
{
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_alltoallv_persist, Alltoallv_persist)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_alltoallv_pure_persist, Alltoallv_pure_persist)
{
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_reduce_persist, Reduce_persist)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_reduce_pure_persist, Reduce_pure_persist)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_barrier_persist, Barrier_persist)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(SYNC);
    return true;
}

BENCHMARK(IMB_barrier_pure_persist, Barrier_pure_persist)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SYNC);
    return true;
}