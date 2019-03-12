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

#define BENCHMARK(BMRK_FN, BMRK_NAME) template class OriginalBenchmark<BenchmarkSuite<BS_MPI1>, BMRK_FN>; \
DECLARE_INHERITED_TEMPLATE(GLUE_TYPENAME(OriginalBenchmark<BenchmarkSuite<BS_MPI1>, BMRK_FN>), BMRK_NAME) \
template<> smart_ptr<Bmark_descr> OriginalBenchmark<BenchmarkSuite<BS_MPI1>, BMRK_FN>::descr = NULL; \
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
