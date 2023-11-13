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
template<> smart_ptr<Bmark_descr> OriginalBenchmark<BenchmarkSuite<BS_NBC>, BMRK_FN>::descr = NULL; \
DECLARE_INHERITED_TEMPLATE(GLUE_TYPENAME(OriginalBenchmark<BenchmarkSuite<BS_NBC>, BMRK_FN>), BMRK_NAME) \
template class OriginalBenchmark<BenchmarkSuite<BS_NBC>, BMRK_FN>; \
template<> bool OriginalBenchmark<BenchmarkSuite<BS_NBC>, BMRK_FN>::init_description() 

BENCHMARK(IMB_ibcast, Ibcast)
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

BENCHMARK(IMB_ibcast_pure, Ibcast_pure)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_iallgather, Iallgather)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_iallgather_pure, Iallgather_pure)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_iallgatherv, Iallgatherv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_iallgatherv_pure, Iallgatherv_pure)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_igather, Igather)
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

BENCHMARK(IMB_igather_pure, Igather_pure)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_igatherv, Igatherv)
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

BENCHMARK(IMB_igatherv_pure, Igatherv_pure)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_iscatter, Iscatter)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_iscatter_pure, Iscatter_pure)
{
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_iscatterv, Iscatterv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_iscatterv_pure, Iscatterv_pure)
{
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_ialltoall, Ialltoall)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_ialltoall_pure, Ialltoall_pure)
{
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_ialltoallv, Ialltoallv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_ialltoallv_pure, Ialltoallv_pure)
{
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_ireduce, Ireduce)
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

BENCHMARK(IMB_ireduce_pure, Ireduce_pure)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_ireduce_scatter, Ireduce_scatter)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_ireduce_scatter_pure, Ireduce_scatter_pure)
{
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_iallreduce, Iallreduce)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_iallreduce_pure, Iallreduce_pure)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(COLLECTIVE);
    return true;
}

BENCHMARK(IMB_ibarrier, Ibarrier)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(SYNC);
    return true;
}

BENCHMARK(IMB_ibarrier_pure, Ibarrier_pure)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SYNC);
    return true;
}
