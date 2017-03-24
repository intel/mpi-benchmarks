#include <vector>
#include <string>
#include <map>
#include <set>
#include <iostream>

#include "MT_benchmark.h" 
#include "MT_suite.h"

#define GLUE_TYPENAME2(A,B) A,B
#define GLUE_TYPENAME3(A,B,C) A,B,C

using namespace std;

inline bool set_stride(int rank, int size, int &stride, int &group)
{
    if (stride == 0)
        stride = size/2;
    if (stride <= 0 || stride > size/2)
        return false;
    group = rank / stride;
    if ((group / 2 == size / (2 * stride)) && (size % (2*stride) != 0))
        return false;
    return true;
}

template <bool set_src, int tag>
int immb_pt2pt(int repeat, void *in, void *out, int count, MPI_Datatype type,
               MPI_Comm comm, int rank, int size, int root, int stride) {

    int group = 0;
    if (!set_stride(rank, size, stride, group))
        return 0;
    for (int i = 0; i < repeat; i++)
        if (group % 2 == 0) {
            MPI_Send(in, count, type, rank+stride, (tag == MPI_ANY_TAG ? 0 : tag), comm);
            MPI_Recv(out, count, type, set_src ? (rank+stride) : MPI_ANY_SOURCE, tag, comm, MPI_STATUS_IGNORE);
        } else {
            MPI_Recv(out, count, type, set_src ? (rank-stride) : MPI_ANY_SOURCE, tag, comm, MPI_STATUS_IGNORE);
            MPI_Send(in, count, type, rank-stride, (tag == MPI_ANY_TAG ? 0 : tag), comm);
        }
    return 1;
}


template class PingPongMT<MTBenchmarkSuite, immb_pt2pt<true, 0> >;
DECLARE_INHERITED(GLUE_TYPENAME3(PingPongMT<MTBenchmarkSuite, immb_pt2pt<true, 0> >), PingPongMT)

//template class PingPongMT<MTBenchmarkSuite, immb_pt2pt<true, 0> >;
//DECLARE_INHERITED(GLUE_TYPENAME3(PingPongMT<MTBenchmarkSuite, immb_pt2pt<true, 0> >), PingPongMTSpecificSourceSpecificTag)

template class PingPongMT<MTBenchmarkSuite, immb_pt2pt<true, MPI_ANY_TAG> >;
DECLARE_INHERITED(GLUE_TYPENAME3(PingPongMT<MTBenchmarkSuite, immb_pt2pt<true, MPI_ANY_TAG> >), 
                  PingPongMTSpecificSourceAnyTag)

template class PingPongMT<MTBenchmarkSuite, immb_pt2pt<false, 0> >;
DECLARE_INHERITED(GLUE_TYPENAME3(PingPongMT<MTBenchmarkSuite, immb_pt2pt<false, 0> >), 
                  PingPongMTAnySourceSpecificTag)

template class PingPongMT<MTBenchmarkSuite, immb_pt2pt<false, MPI_ANY_TAG> >;
DECLARE_INHERITED(GLUE_TYPENAME3(PingPongMT<MTBenchmarkSuite, immb_pt2pt<false, MPI_ANY_TAG> >), 
                  PingPongMTAnySourceAnyTag)

template <bool set_src, int tag>
int immb_ipt2pt(int repeat, void *in, void *out, int count, MPI_Datatype type,
                MPI_Comm comm, int rank, int size, int root, int stride) {
    int group = 0;
    if (!set_stride(rank, size, stride, group))
        return 0;
    MPI_Request request;
    int dest = (group % 2 == 0 ? rank+stride : rank-stride);
    for (int i = 0; i < repeat; i++) {
        MPI_Isend(in, count, type, dest, tag == MPI_ANY_TAG ? 0 : tag, comm, &request);
        MPI_Recv(out, count, type, set_src ? dest : MPI_ANY_SOURCE, tag, comm, MPI_STATUS_IGNORE);
        MPI_Wait(&request, MPI_STATUS_IGNORE);
    }
    return 1;
}

template class PingPongMT<MTBenchmarkSuite, immb_ipt2pt<true, 0> >;
DECLARE_INHERITED(GLUE_TYPENAME3(PingPongMT<MTBenchmarkSuite, immb_ipt2pt<true, 0> >), PingPingMT)

template <bool set_src, int tag>
int immb_sendrecv(int repeat, void *in, void *out, int count, MPI_Datatype type,
                  MPI_Comm comm, int rank, int size, int root, int stride) {
    int dest = (rank + 1) % size;
    int src = (rank + size - 1) % size;
    for (int i = 0; i < repeat; i++)
        MPI_Sendrecv(in, count, type, dest, (tag == MPI_ANY_TAG ? 0 : tag),
                     out, count, type, set_src ? src : MPI_ANY_SOURCE, tag, comm, MPI_STATUS_IGNORE);   
    }
    return 1;
}

template class PingPongMT<MTBenchmarkSuite, immb_sendrecv<true, 0> >;
DECLARE_INHERITED(GLUE_TYPENAME3(PingPongMT<MTBenchmarkSuite, immb_sendrecv<true, 0> >), SendRecvMT)

int immb_exchange(int repeat, void *in, void *out, int count, MPI_Datatype type,
                  MPI_Comm comm, int rank, int size, int root, int stride) {
    int tag = 0;
    int right = rank + 1;
    int left = rank - 1;
    if (right == size) right = 0;
    if (left == -1) left = size - 1;
    MPI_Request requests[2];
    for (int i = 0; i < repeat; i++)
        MPI_Isend(in, count, type, left, tag, comm, &requests[0]);
        MPI_Isend(in, count, type, right, tag, comm, &requests[1]);
        MPI_Recv(out, count, type, left, tag, comm, MPI_STATUS_IGNORE);
        MPI_Recv(out, count, type, right, tag, comm, MPI_STATUS_IGNORE);
        MPI_Waitall(2, requests, MPI_STATUSES_IGNORE);
    }
    return 1;
}

template class PingPongMT<MTBenchmarkSuite, immb_exchange>;
DECLARE_INHERITED(GLUE_TYPENAME3(PingPongMT<MTBenchmarkSuite, immb_exchgange>), ExchangeMT)

template <bool set_src, int tag>    
int immb_uniband(int repeat, void *in, void *out, int count, MPI_Datatype type,
                 MPI_Comm comm, int rank, int size, int root, int stride) {
    int group = 0;
    if (!set_stride(rank, size, stride, group))
        return 0;
    MPI_Request requests[2 * MAX_WIN_SIZE];
    char ack = 0;
    int right = rank + stride;
    int left = rank - stride;
    for (int i = 0; i < repeat; i++) {
        if (group % 2 == 0) {
            for (int w = 0; w < MAX_WIN_SIZE; w++) {
                MPI_Irecv(out, count, type, set_src ? right : MPI_ANY_SOURCE, tag, comm, &requests[w]);
            }
            for (int w = 0; w < MAX_WIN_SIZE; w++) {
                MPI_Isend(in, count, type, right, (tag == MPI_ANY_TAG ? 0 : tag), comm, &requests[w + MAX_WIN_SIZE]);
            }
            MPI_Waitall(2 * MAX_WIN_SIZE, requests, MPI_STATUSES_IGNORE);
            MPI_Recv(&ack, 1, MPI_CHAR, recv, tag, comm, MPI_STATUS_IGNORE);
        } else {
            for (int w = 0; w < MAX_WIN_SIZE; w++) {
                MPI_Irecv(out, count, type, set_src ? left : MPI_ANY_SOURCE, tag, comm, &requests[w]);
            }
            for (int w = 0; w < MAX_WIN_SIZE; w++) {
                MPI_Isend(in, count, type, left, (tag == MPI_ANY_TAG ? 0 : tag), comm, &requests[w + MAX_WIN_SIZE]);
            }
            MPI_Waitall(2 * MAX_WIN_SIZE, requests, MPI_STATUSES_IGNORE);
            MPI_Send(&ack, 1, MPI_CHAR, left, (tag == MPI_ANY_TAG ? 0 : tag), comm, MPI_STATUS_IGNORE);
        }
    }
    return 1;
}

