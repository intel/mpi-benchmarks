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

#include "benchmark_suites_collection.h"
#include "utils.h"
#include "benchmark_suite.h"
#include "MT_types.h"
#include "MT_benchmark.h"

#define WITH_VECTOR

#define GLUE_TYPENAME2(A,B) A,B
#define GLUE_TYPENAME3(A,B,C) A,B,C

#define WRAP(NEWNAME, OLDNAME)                                                              \
    int NEWNAME(int repeat, int skip, void *in, void *out, int count, MPI_Datatype type,    \
                MPI_Comm comm, int rank, int size, input_benchmark_data *idata,             \
                output_benchmark_data *odata) {                                             \
        return OLDNAME(repeat, skip, in, out, count, type, comm, rank, size, idata, odata); \
    }

#define DECLARE_INHERITED_BENCHMARKMT2(BS, FUNC, NAME) template class BenchmarkMT<BS, FUNC>; \
    DECLARE_INHERITED_TEMPLATE(GLUE_TYPENAME3(BenchmarkMT<BS, FUNC>), NAME)                  \
    template <> void BenchmarkMT<BS, FUNC >::init_flags()

#define DECLARE_INHERITED_BENCHMARKMT(BS, FUNC, NAME) template class BenchmarkMT<BS, FUNC>; \
    DECLARE_INHERITED_TEMPLATE(GLUE_TYPENAME2(BenchmarkMT<BS, FUNC>), NAME)                 \
    template <> void BenchmarkMT<BS, FUNC >::init_flags()


/* testing convenience macros */
#define INIT_ARRAY(cond,arr,val)                                     \
    if (idata->checks.check && (cond)) {                             \
        int type_size;                                               \
        MPI_Type_size(type, &type_size);                             \
        for (size_t i = 0; i < count * type_size / sizeof(int); i++) \
            ((int *)(arr))[i] = (int)(val);                          \
    }

#define CHECK_ARRAY(cond,arr,val)                                                                     \
    if (idata->checks.check && (cond)) {                                                              \
        int type_size;                                                                                \
        MPI_Type_size(type, &type_size);                                                              \
        for (size_t i = 0; i < count * type_size / sizeof(int); i++)                                  \
            if( ((int *)(arr))[i] != (int)(val) ) {                                                   \
                odata->checks.failures++;                                                             \
            }                                                                                         \
    }

using namespace std;

inline bool set_stride(int rank, int size, int &stride, int &group)
{
    if (stride == 0)
        stride = size / 2;
    if (stride <= 0 || stride > size / 2)
        return false;
    group = rank / stride;
    if ((group / 2 == size / (2 * stride)) && (size % (2 * stride) != 0))
        return false;
    return true;
}

template <bool set_src, int tag>
int mt_pt2pt(int repeat, int, void *in, void *out, int count, MPI_Datatype type,
               MPI_Comm comm, int rank, int size, input_benchmark_data *idata,
               output_benchmark_data *odata) {
    int group = 0;
    int stride = idata->pt2pt.stride;
    if (!set_stride(rank, size, stride, group))
        return 0;
    INIT_ARRAY(true, in, (rank + 1) * i);
    INIT_ARRAY(true, out, -1);
    int pair = -1;
    for (int i = 0; i < repeat; i++)
        if (group % 2 == 0) {
            pair = rank + stride;
            MPI_Send(in, count, type, pair, (tag == MPI_ANY_TAG ? 0 : tag), comm);
            MPI_Recv(out, count, type, set_src ? pair : MPI_ANY_SOURCE, tag, comm, MPI_STATUS_IGNORE);
        } else {
            pair = rank - stride;
            MPI_Recv(out, count, type, set_src ? pair : MPI_ANY_SOURCE, tag, comm, MPI_STATUS_IGNORE);
            MPI_Send(in, count, type, pair, (tag == MPI_ANY_TAG ? 0 : tag), comm);
        }
    CHECK_ARRAY(true, in, (rank + 1) * i);
    CHECK_ARRAY(true, out, (pair + 1) * i);
    return 1;
}

DECLARE_INHERITED_BENCHMARKMT2(BenchmarkSuite<BS_MT>, GLUE_TYPENAME2(mt_pt2pt<true, 0>), PingPongMT) {
    flags.insert(PT2PT);
    flags.insert(TIME_DIVIDE_BY_2);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
    flags.insert(OUT_BW);
}
#if 0

WRAP(mt_pt2pt_SSST, GLUE_TYPENAME2(mt_pt2pt<true, 0>))

template class BenchmarkMT<MTBenchmarkSuite, mt_pt2pt<true, 0> >;
namespace { BenchmarkMT<MTBenchmarkSuite, mt_pt2pt<true, 0> > elem_aaa; } template<> const char *BenhmarkMT<MTBenchmarkSuite, mt_pt2pt<true, 0> >::name = "PingPongMT";
template <> void BenchmarkMT<MTBenchmarkSuite, mt_pt2pt<true, 0> >::init_flags()

//DECLARE_INHERITED_BENCHMARKMT(MTBenchmarkSuite, mt_pt2pt_SSST, PingPongMTSpecificSourceSpecificTag) {
{
    flags.insert(PT2PT);
    flags.insert(TIME_DIVIDE_BY_2);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
    flags.insert(OUT_BW);
}

DECLARE_INHERITED_BENCHMARKMT2(MTBenchmarkSuite, GLUE_TYPENAME2(mt_pt2pt<true, MPI_ANY_TAG>), PingPongMTSpecificSourceAnyTag)
{
    flags.insert(PT2PT);
    flags.insert(TIME_DIVIDE_BY_2);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
    flags.insert(OUT_BW);
}

DECLARE_INHERITED_BENCHMARKMT2(MTBenchmarkSuite, GLUE_TYPENAME2(mt_pt2pt<false, 0>), PingPongMTAnySourceSpecificTag)
{
    flags.insert(PT2PT);
    flags.insert(TIME_DIVIDE_BY_2);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
    flags.insert(OUT_BW);
}

DECLARE_INHERITED_BENCHMARKMT2(MTBenchmarkSuite, GLUE_TYPENAME2(mt_pt2pt<false, MPI_ANY_TAG>), PingPongMTAnySourceAnyTag)
{
    flags.insert(PT2PT);
    flags.insert(TIME_DIVIDE_BY_2);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
    flags.insert(OUT_BW);
}
#endif

template <bool set_src, int tag>
int mt_ipt2pt(int repeat, int, void *in, void *out, int count, MPI_Datatype type,
                MPI_Comm comm, int rank, int size, input_benchmark_data *idata,
                output_benchmark_data *odata) {
    int group = 0;
    int stride = idata->pt2pt.stride;
    if (!set_stride(rank, size, stride, group))
        return 0;
    INIT_ARRAY(true, in, (rank+1)*i);
    INIT_ARRAY(true, out, -1);
    MPI_Request request;
    int dest = (group % 2 == 0 ? rank+stride : rank-stride);
    for (int i = 0; i < repeat; i++) {
        MPI_Isend(in, count, type, dest, tag == MPI_ANY_TAG ? 0 : tag, comm, &request);
        MPI_Recv(out, count, type, set_src ? dest : MPI_ANY_SOURCE, tag, comm, MPI_STATUS_IGNORE);
        MPI_Wait(&request, MPI_STATUS_IGNORE);
    }
    CHECK_ARRAY(true, in, (rank+1)*i);
    CHECK_ARRAY(true, out, (dest+1)*i);
    return 1;
}

DECLARE_INHERITED_BENCHMARKMT2(BenchmarkSuite<BS_MT>, GLUE_TYPENAME2(mt_ipt2pt<true, 0>), PingPingMT)
{
    flags.insert(PT2PT);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
    flags.insert(OUT_BW);
}

template <bool set_src, int tag>
int mt_sendrecv(int repeat, int, void *in, void *out, int count, MPI_Datatype type,
                  MPI_Comm comm, int rank, int size, input_benchmark_data *idata,
                  output_benchmark_data *odata) {
    int group = 0;
    int stride = idata->pt2pt.stride;
    set_stride(rank, size, stride, group);
    INIT_ARRAY(true, in, (rank + 1) * i);
    INIT_ARRAY(true, out, -1);
    int dest = (rank + stride) % size;
    int src = (rank + size - stride) % size;
    for (int i = 0; i < repeat; i++) {
        MPI_Sendrecv(in, count, type, dest, (tag == MPI_ANY_TAG ? 0 : tag),
                     out, count, type, set_src ? src : MPI_ANY_SOURCE, tag, comm, MPI_STATUS_IGNORE);
    }
    CHECK_ARRAY(true, in, (rank + 1) * i);
    CHECK_ARRAY(true, out, (src + 1) * i);
    return 1;
}
DECLARE_INHERITED_BENCHMARKMT2(BenchmarkSuite<BS_MT>, GLUE_TYPENAME2(mt_sendrecv<true, 0>), SendRecvMT)
{
    flags.insert(PT2PT);
    flags.insert(SCALE_BW_TWICE);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
    flags.insert(OUT_BW);
}

void *increment_ptr(void *p, int count, MPI_Datatype type)
{
    int type_size = 0;
    MPI_Type_size(type, &type_size);
    return (char *)p + count * type_size;
}

int mt_exchange(int repeat, int, void *in, void *out, int count, MPI_Datatype type,
                  MPI_Comm comm, int rank, int size, input_benchmark_data *idata,
                  output_benchmark_data *odata) {
    int group = 0;
    int stride = idata->pt2pt.stride;
    set_stride(rank, size, stride, group);
    void *out2 = increment_ptr(out, count, type);
    INIT_ARRAY(true, in, (rank + 1) * i);
    INIT_ARRAY(true, out, -1);
    int tag = 0;
    int right = (rank + stride) % size;
    int left = (rank + size - stride) % size;
    MPI_Request requests[2];
    for (int i = 0; i < repeat; i++) {
        MPI_Isend(in, count, type, left, tag, comm, &requests[0]);
        MPI_Isend(in, count, type, right, tag, comm, &requests[1]);
        MPI_Recv(out, count, type, left, tag, comm, MPI_STATUS_IGNORE);
        MPI_Recv(out2, count, type, right, tag, comm, MPI_STATUS_IGNORE);
        MPI_Waitall(2, requests, MPI_STATUSES_IGNORE);
    }
    CHECK_ARRAY(true, in, (rank + 1) * i);
    CHECK_ARRAY(true, out, (left + 1) * i);
    CHECK_ARRAY(true, out2, (right + 1) * i);
    return 1;
}

DECLARE_INHERITED_BENCHMARKMT(BenchmarkSuite<BS_MT>, mt_exchange, ExchangeMT)
{
    flags.insert(PT2PT);
    flags.insert(RECV_FROM_2);
    flags.insert(SCALE_BW_FOUR);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
    flags.insert(OUT_BW);
}

template <bool set_src, int tag>
int mt_uniband(int repeat, int, void *in, void *out, int count, MPI_Datatype type,
                 MPI_Comm comm, int rank, int size, input_benchmark_data *idata,
                 output_benchmark_data *odata) {
    int group = 0;
    int stride = idata->pt2pt.stride;
    if (!set_stride(rank, size, stride, group))
        return 0;
    int window_size = idata->window.size;
    MPI_Request *requests = idata->window.requests;
    char ack = 0;
    INIT_ARRAY(true, in, (rank + 1) * i);
    INIT_ARRAY(true, out, -1);
    int right = rank + stride;
    int left = rank - stride;
    for (int i = 0; i < repeat; i++) {
        if (group % 2 == 0) {
            for (int w = 0; w < window_size; w++) {
                MPI_Isend(in, count, type, right, (tag == MPI_ANY_TAG ? 0 : tag), comm, &requests[w]);
            }
            MPI_Waitall(window_size, requests, MPI_STATUSES_IGNORE);
            MPI_Recv(&ack, 1, MPI_CHAR, right, tag, comm, MPI_STATUS_IGNORE);
        } else {
            for (int w = 0; w < window_size; w++) {
                MPI_Irecv(out, count, type, set_src ? left : MPI_ANY_SOURCE, tag, comm, &requests[w]);
            }
            MPI_Waitall(window_size, requests, MPI_STATUSES_IGNORE);
            MPI_Send(&ack, 1, MPI_CHAR, left, (tag == MPI_ANY_TAG ? 0 : tag), comm);
        }
    }
    CHECK_ARRAY(true, in, (rank + 1) * i);
    CHECK_ARRAY(group % 2 == 1, out, (left + 1) * i);
    return 1;
}

DECLARE_INHERITED_BENCHMARKMT2(BenchmarkSuite<BS_MT>, GLUE_TYPENAME2(mt_uniband<true, 0>), UniBandMT)
{
    flags.insert(PT2PT);
    flags.insert(WINDOW);
    flags.insert(TIME_DIVIDE_BY_WINDOW_SIZE);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_AVG);
    flags.insert(OUT_BW_CUMULATIVE);
    flags.insert(OUT_MSGRATE_CUMMULATIVE);
}

template <bool set_src, int tag>
int mt_biband(int repeat, int, void *in, void *out, int count, MPI_Datatype type,
                 MPI_Comm comm, int rank, int size, input_benchmark_data *idata,
                 output_benchmark_data *odata) {
    int group = 0;
    int stride = idata->pt2pt.stride;
    if (!set_stride(rank, size, stride, group))
        return 0;
    int window_size = idata->window.size;
    MPI_Request *requests = idata->window.requests;
    char ack = 0;
    INIT_ARRAY(1, in, (rank + 1) * i);
    INIT_ARRAY(1, out, -1);
    int right = rank + stride;
    int left = rank - stride;
    for (int i = 0; i < repeat; i++) {
        if (group % 2 == 0) {
            for (int w = 0; w < window_size; w++) {
                MPI_Irecv(out, count, type, set_src ? right : MPI_ANY_SOURCE, tag, comm, &requests[w]);
            }
            for (int w = 0; w < window_size; w++) {
                MPI_Isend(in, count, type, right, (tag == MPI_ANY_TAG ? 0 : tag), comm, &requests[w + window_size]);
            }
            MPI_Waitall(2 * window_size, requests, MPI_STATUSES_IGNORE);
            MPI_Recv(&ack, 1, MPI_CHAR, right, tag, comm, MPI_STATUS_IGNORE);
        } else {
            for (int w = 0; w < window_size; w++) {
                MPI_Irecv(out, count, type, set_src ? left : MPI_ANY_SOURCE, tag, comm, &requests[w]);
            }
            for (int w = 0; w < window_size; w++) {
                MPI_Isend(in, count, type, left, (tag == MPI_ANY_TAG ? 0 : tag), comm, &requests[w + window_size]);
            }
            MPI_Waitall(2 * window_size, requests, MPI_STATUSES_IGNORE);
            MPI_Send(&ack, 1, MPI_CHAR, left, (tag == MPI_ANY_TAG ? 0 : tag), comm);
        }
    }
    CHECK_ARRAY(true, in, (rank + 1) * i);
    CHECK_ARRAY(group % 2 == 0, out, (right + 1) * i);
    CHECK_ARRAY(group % 2 == 1, out, (left + 1) * i);
    return 1;
}

DECLARE_INHERITED_BENCHMARKMT2(BenchmarkSuite<BS_MT>, GLUE_TYPENAME2(mt_biband<true, 0>), BiBandMT)
{
    flags.insert(PT2PT);
    flags.insert(WINDOW);
    flags.insert(TIME_DIVIDE_BY_2);
    flags.insert(TIME_DIVIDE_BY_WINDOW_SIZE);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_AVG);
    flags.insert(OUT_BW_CUMULATIVE);
    flags.insert(OUT_MSGRATE_CUMMULATIVE);
}
#define MT_COLLECTIVE_BEGIN(NAME) int mt_##NAME(int repeat, int skip, void *in, void *out, int count, MPI_Datatype type, \
               MPI_Comm comm, int rank, int size, input_benchmark_data *idata, output_benchmark_data *odata) 

#define MT_CYCLE_BEGIN \
    double t, sum = 0.0; \
    for (int i = 0; i < repeat+skip; i++) { \
        t = MPI_Wtime(); 

#define MT_CYCLE_END        t = MPI_Wtime() - t; \
        if (i >= skip) sum += t; \
        idata->barrier.fn_ptr(); \
        idata->barrier.fn_ptr(); \
        idata->barrier.fn_ptr(); \
        idata->barrier.fn_ptr(); \
        idata->barrier.fn_ptr(); \
    } \
    if (odata->timing.time_ptr != NULL)  { \
        *(odata->timing.time_ptr) = sum; \
    } \

#define MT_CYCLE_END_NOBARRIER   t = MPI_Wtime() - t; \
        if (i >= skip) sum += t; \
    } \
    if (odata->timing.time_ptr != NULL)  { \
        *(odata->timing.time_ptr) = sum; \
    } \


MT_COLLECTIVE_BEGIN(barrier) {
    UNUSED(size);
    UNUSED(in);
    UNUSED(out);
    UNUSED(count);
    UNUSED(type);
    UNUSED(rank);
    MT_CYCLE_BEGIN
        if (idata->threading.mode_multiple) {
#pragma omp barrier
        }
        MPI_Barrier(comm);
    MT_CYCLE_END_NOBARRIER
    return 1;
}

DECLARE_INHERITED_BENCHMARKMT(BenchmarkSuite<BS_MT>, mt_barrier, BarrierMT)
{
    flags.insert(COLLECTIVE);
    flags.insert(SEPARATE_MEASURING);
    flags.insert(SEND_0);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);

}

MT_COLLECTIVE_BEGIN(bcast) {
    UNUSED(size);
    INIT_ARRAY((idata->collective.root == rank), in, i);
    INIT_ARRAY((idata->collective.root != rank), out, -1);
    MT_CYCLE_BEGIN
        MPI_Bcast((idata->collective.root == rank) ? in : out, count, type, idata->collective.root, comm);
    MT_CYCLE_END
    CHECK_ARRAY((idata->collective.root != rank), out, i);
    return 1;
}

DECLARE_INHERITED_BENCHMARKMT(BenchmarkSuite<BS_MT>, mt_bcast, BcastMT)
{
    flags.insert(COLLECTIVE);
    flags.insert(SEPARATE_MEASURING);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);

}


MT_COLLECTIVE_BEGIN(reduce) {
    INIT_ARRAY(1, in, (rank+1)*i);
    INIT_ARRAY(1, out, -1);
    MT_CYCLE_BEGIN
        MPI_Reduce(in, out, count, type, MPI_SUM, idata->collective.root, comm);
    MT_CYCLE_END
    CHECK_ARRAY((idata->collective.root == rank), out, size*(size+1)*i/2);
    return 1;
}

DECLARE_INHERITED_BENCHMARKMT(BenchmarkSuite<BS_MT>, mt_reduce, ReduceMT)
{
    flags.insert(COLLECTIVE);
    flags.insert(SEPARATE_MEASURING);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
}


MT_COLLECTIVE_BEGIN(allreduce) {
    INIT_ARRAY(1, in, (rank+1)*i);
    INIT_ARRAY(1, out, -1);
    MT_CYCLE_BEGIN
        MPI_Allreduce(in, out, count, type, MPI_SUM, comm);
    MT_CYCLE_END
    CHECK_ARRAY(true, out, size*(size+1)*i/2);
    return 1;
}

DECLARE_INHERITED_BENCHMARKMT(BenchmarkSuite<BS_MT>, mt_allreduce, AllReduceMT)
{
    flags.insert(COLLECTIVE);
    flags.insert(SEPARATE_MEASURING);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
}


#if 0
MT_COLLECTIVE_BEGIN(scatter)
    MPI_Scatter(in, count, type, out, count, type, idata->collective.root, comm);
MT_COLLECTIVE_END(scatter)

DECLARE_INHERITED_BENCHMARKMT(MTBenchmarkSuite, mt_scatter, ScatterMT)
{
    flags.insert(COLLECTIVE);
    flags.insert(SEPARATE_MEASURING);
    flags.insert(SEND_TO_ALL);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
}


#ifdef WITH_VECTOR
MT_COLLECTIVE_BEGIN(scatterv)
    MPI_Scatterv(in, idata->collective_vector.cnt, idata->collective_vector.displs, type, out, count, type, idata->collective.root, comm);
MT_COLLECTIVE_END(scatterv)

DECLARE_INHERITED_BENCHMARKMT(MTBenchmarkSuite, mt_scatterv, ScattervMT)
{
    flags.insert(COLLECTIVE);
    flags.insert(SEPARATE_MEASURING);
    flags.insert(COLLECTIVE_VECTOR);
    flags.insert(SEND_TO_ALL);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
}


MT_COLLECTIVE_BEGIN(reduce_scatter)
    MPI_Reduce_scatter(in, out, idata->collective_vector.cnt, type, MPI_SUM, comm);
MT_COLLECTIVE_END(reduce_scatter)

DECLARE_INHERITED_BENCHMARKMT(MTBenchmarkSuite, mt_reduce_scatter, ReduceScatterMT)
{
    flags.insert(COLLECTIVE);
    flags.insert(SEPARATE_MEASURING);
    flags.insert(COLLECTIVE_VECTOR);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
}
#endif

MT_COLLECTIVE_BEGIN(gather)
    MPI_Gather(in, count, type, out, count, type, idata->collective.root, comm);
MT_COLLECTIVE_END(gather)

DECLARE_INHERITED_BENCHMARKMT(MTBenchmarkSuite, mt_gather, GatherMT)
{
    flags.insert(COLLECTIVE);
    flags.insert(SEPARATE_MEASURING);
    flags.insert(RECV_FROM_ALL);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
}


#ifdef WITH_VECTOR
MT_COLLECTIVE_BEGIN(gatherv)
    MPI_Gatherv(in, count, type, out, idata->collective_vector.cnt, idata->collective_vector.displs, type, idata->collective.root, comm);
MT_COLLECTIVE_END(gatherv)

DECLARE_INHERITED_BENCHMARKMT(MTBenchmarkSuite, mt_gatherv, GathervMT)
{
    flags.insert(COLLECTIVE);
    flags.insert(SEPARATE_MEASURING);
    flags.insert(COLLECTIVE_VECTOR);
    flags.insert(RECV_FROM_ALL);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
}
#endif

MT_COLLECTIVE_BEGIN(allgather)
    MPI_Allgather(in, count, type, out, count, type, comm);
MT_COLLECTIVE_END(allgather)

DECLARE_INHERITED_BENCHMARKMT(MTBenchmarkSuite, mt_allgather, AllgatherMT)
{
    flags.insert(COLLECTIVE);
    flags.insert(SEPARATE_MEASURING);
    flags.insert(RECV_FROM_ALL);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
}


#ifdef WITH_VECTOR
MT_COLLECTIVE_BEGIN(allgatherv)
    MPI_Allgatherv(in, count, type, out, idata->collective_vector.cnt, idata->collective_vector.displs, type, comm);
MT_COLLECTIVE_END(allgatherv)

DECLARE_INHERITED_BENCHMARKMT(MTBenchmarkSuite, mt_allgatherv, AllgathervMT)
{
    flags.insert(COLLECTIVE);
    flags.insert(SEPARATE_MEASURING);
    flags.insert(COLLECTIVE_VECTOR);
    flags.insert(RECV_FROM_ALL);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
}
#endif

MT_COLLECTIVE_BEGIN(alltoall)
    MPI_Alltoall(in, count, type, out, count, type, comm);
MT_COLLECTIVE_END(alltoall)

DECLARE_INHERITED_BENCHMARKMT(MTBenchmarkSuite, mt_alltoall, AlltoallMT)
{
    flags.insert(COLLECTIVE);
    flags.insert(SEPARATE_MEASURING);
    flags.insert(SEND_TO_ALL);
    flags.insert(RECV_FROM_ALL);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
}


#ifdef WITH_VECTOR
MT_COLLECTIVE_BEGIN(alltoallv)
    MPI_Alltoallv(in,  idata->collective_vector.cnt,  idata->collective_vector.displs, type, out, idata->collective_vector.cnt, idata->collective_vector.displs, type, comm);
MT_COLLECTIVE_END(alltoallv)

DECLARE_INHERITED_BENCHMARKMT(MTBenchmarkSuite, mt_alltoallv, AlltoallvMT)
{
    flags.insert(COLLECTIVE);
    flags.insert(SEPARATE_MEASURING);
    flags.insert(COLLECTIVE_VECTOR);
    flags.insert(SEND_TO_ALL);
    flags.insert(RECV_FROM_ALL);
    flags.insert(OUT_BYTES);
    flags.insert(OUT_REPEAT);
    flags.insert(OUT_TIME_MIN);
    flags.insert(OUT_TIME_MAX);
    flags.insert(OUT_TIME_AVG);
}
#endif

#endif

#if 0

#include "ransac.cpp"
#include <math.h>

int mt_bcast(int repeat, void *in, void *out, int count, MPI_Datatype type,
               MPI_Comm comm, int rank, int size, int root, int stride, double *t_avg) {
    double t, sum = 0.0;
    for (int i = 0; i < repeat; i++) {
        t = MPI_Wtime();
        MPI_Bcast(in, count, type, 0, comm);
        t = MPI_Wtime() - t;
        sum += t;
        MPI_Barrier(comm);
        MPI_Barrier(comm);
        MPI_Barrier(comm);
        MPI_Barrier(comm);
        MPI_Barrier(comm);
    }
    sum /= repeat;
    *t_avg = sum;
}

#if 0
int get_token(double t)
{
  double result;
  int n;
  result = frexp(t , &n);
  return (int)floor(result*1000.0) * 100 + abs(n);
}
#endif

int mt_bcast(int repeat, void *in, void *out, int count, MPI_Datatype type,
               MPI_Comm comm, int rank, int size, int root, int stride) {
    static int prev_count = 0;
    double t, sum = 0.0;
#if 0
    vector<double> times;
    typedef pair<double, int> value;
    times.resize(repeat);
    map<int, value> times_map;
#endif
    for (int i = 0; i < repeat; i++) {
        t = MPI_Wtime();
        MPI_Bcast(in, count, type, 0, comm);
        t = MPI_Wtime() - t;
        sum += t;
#if 0
        times[i] = t*1000000.0
        int token = get_token(t*1000000.0);
        value old_val = times_map[token];
        times_map[token] = value(t*1000000.0, old_val.second + 1);
#endif
        MPI_Barrier(comm);
        MPI_Barrier(comm);
        MPI_Barrier(comm);
        MPI_Barrier(comm);
        MPI_Barrier(comm);
    }
    // FIXME!!! use this value
    sum /= repeat;

    if (prev_count == count /*&& repeat > 40*/) {
//        int instability_mark = 0;
#if 0
        // ransac
        {
            double avg = 0;
            for (int i = 0; i < times.size(); i++) {
                cout << "<< " << times[i] << endl;
            }
            int niter = 0;
            double threshold = (times[0]+times[2]+times[3]+times[4])/4.0/5.0;
            Model<double> M = ransac<double, Model<double> >(times, max(threshold, 0.1), 0.99, niter);
            if (niter > 50)
                instability_mark++;
            if (niter > 200)
                instability_mark++;
            MPI_Reduce(&M.N, &avg, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
            if (rank == 0) {
                cout << ">> ransac: " << avg / size << endl;
            }
        }
#endif
#if 0
        // simple avegare
        {
            double avg = 0;
            Model<double> S(0); S.init(times);
            MPI_Reduce(&S.N, &avg, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
            if (rank == 0) {
                cout << ">> avg: " << avg / size << endl;
            }
        }
#endif
#if 0
        // frequency-based
        {
            map<int, double> sorted;
            for (map<int, value>::iterator it = times_map.begin(); it != times_map.end(); ++it) {
                sorted[it->second.second] = it->second.first;
            }
            int prev = 0;
            vector<double> top_vals;
            vector<int> top_tokens;
            vector<int> top_counts;
            for (map<int, double>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
                if (it->first < prev*2/3) {
                    break;
                }
                top_vals.push_back(it->second);
                cout << ">>>> " << it->second << " " << it->first << endl;
                top_tokens.push_back(get_token(it->second));
                top_counts.push_back(it->first);
                prev = it->first;
                if (top_vals.size() > 5)
                    break;
            }
            if (top_vals.size() > 3) {
                instability_mark++;
            }
            if (top_vals.size() > 5) {
                instability_mark++;
            }
            if (top_vals.size() > 3) {
                top_vals.resize(0);
#if 0
                Model<double> S(0); S.init(times);
                top_vals.resize(1);
                top_vals[0] = S.N;
                top_counts[0] = times.size()/2;
                top_tokens[0] = get_token(S.N);
#endif
            } else {
                double min_val = 1e6;
                int min_idx = 0;
                for (int i = 0; i < top_vals.size(); i++) {
                    if (top_vals[i] < min_val) {
                        min_val = top_vals[i];
                        min_idx = i;
                    }
                }
                top_vals[0] = top_vals[min_idx];
                top_counts[0] = top_counts[min_idx];
                top_tokens[0] = top_tokens[min_idx];
                top_vals.resize(1);
            }
            int tsize = top_vals.size();
            top_tokens.resize(tsize);
            top_counts.resize(tsize);

            int all_instability_mark = 0;
            MPI_Reduce(&instability_mark, &all_instability_mark, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
            if (rank == 0) {
                cout << ">> instability: " << all_instability_mark << endl;
            }

            int all_tsize = 0;
            vector<int> displs(size + 1, 0);
            MPI_Scan(&tsize, &all_tsize, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
            MPI_Gather(&all_tsize, 1, MPI_INT, &displs[1], 1, MPI_INT, 0, MPI_COMM_WORLD);
            if (rank == 0)
                all_tsize = displs[size];
            else
                all_tsize = 0;
            vector<double> all_top_vals(all_tsize);
            vector<int> all_top_tokens(all_tsize);
            vector<int> all_top_counts(all_tsize);
            vector<int> counts(size);
            for (int i = 0; i < size; i++) {
                counts[i] = displs[i+1] - displs[i];
            }
            MPI_Gatherv(&top_vals[0], tsize, MPI_DOUBLE, &all_top_vals[0],
                        &counts[0], &displs[0], MPI_DOUBLE, 0, MPI_COMM_WORLD);
            MPI_Gatherv(&top_tokens[0], tsize, MPI_INT, &all_top_tokens[0],
                        &counts[0], &displs[0], MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Gatherv(&top_counts[0], tsize, MPI_INT, &all_top_counts[0],
                        &counts[0], &displs[0], MPI_INT, 0, MPI_COMM_WORLD);
            if (rank == 0) {
                double sum = 0, min_val = 1e6, max_val = -1e6;
                int n = 0;
                map<int, value> all_times_map;
                for (int i = 0; i < all_tsize; i++) {
                    cout << "@@ >> " << all_top_tokens[i] << " " << all_top_vals[i] << " " << all_top_counts[i] << endl;
                    int token = all_top_tokens[i];
                    value old_val = all_times_map[token];
                    if (old_val.first != 0 && old_val.first != all_top_vals[0])
                        cout << ">> OOOPS" << endl;
                    old_val.first = all_top_vals[i];
                    old_val.second += all_top_counts[i];
                    all_times_map[token] = old_val;

                    min_val = min(old_val.first, min_val);
                    max_val = max(old_val.first, max_val);
                    sum += old_val.first;
                    n++;
                }
/*
                map<int, double> all_sorted;
                for (map<int, value>::iterator it = all_times_map.begin(); it != all_times_map.end(); ++it) {
                    all_sorted[it->second.second] = it->second.first;
                }
                //double sum = 0;
                //int n = 0, i = 0;
                for (map<int, double>::reverse_iterator it = all_sorted.rbegin(); it != all_sorted.rend(); ++it) {
                    cout << "@@ " << it->second << " " << it->first << endl;
                    sum += it->second;
                    n++;
                    //sum += it->second * it->first;
                    //n += it->first;
//                    if (++i > 8)
//                        break;
                }
*/
//                if (n > 1) { sum -= max_val; n--; }
                cout << ">> freq: " << sum / n << endl;
                cout << ">> minmax: " << min_val << " " << max_val << endl;
            }
        }
#endif
    }
    prev_count = count;
    return 1;
}

//template class PingPongMT<MTBenchmarkSuite, mt_bcast>;
//DECLARE_INHERITED(GLUE_TYPENAME2(PingPongMT<MTBenchmarkSuite, mt_bcast>), BcastMT)

#endif
