#ifdef MPI1

#include <iostream>
#include "orig_benchmark.h"

extern "C" {
#include "IMB_prototypes.h"
}

#include "reworked_IMB_functions.h"

using namespace std;

#define LEGACY_BENCHMARK(LEGACY_BMRK_FN, LEGACY_BMRK_NAME) template class OriginalBenchmark<OriginalBenchmarkSuite_MPI1, LEGACY_BMRK_FN>; \
DECLARE_INHERITED(GLUE_TYPENAME(OriginalBenchmark<OriginalBenchmarkSuite_MPI1, LEGACY_BMRK_FN>), LEGACY_BMRK_NAME) \
reworked_Bmark_descr OriginalBenchmark<OriginalBenchmarkSuite_MPI1, LEGACY_BMRK_FN>::descr; \
bool OriginalBenchmark<OriginalBenchmarkSuite_MPI1, LEGACY_BMRK_FN>::init_descr() 

LEGACY_BENCHMARK(IMB_pingpong, pingpong)
{
    descr.flags.insert(SELECT_SOURCE);
    descr.flags.insert(SINGLE_TRANSFER);
    descr.flags.insert(SCALE_TIME_HALF);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    descr.comments.push_back("This is a pingpong benchmark bla bla bla...");
    return true;
};

LEGACY_BENCHMARK(IMB_pingping, pingping)
{
    descr.flags.insert(SELECT_SOURCE);
    descr.flags.insert(SINGLE_TRANSFER);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    descr.comments.push_back("This is a pingpong benchmark bla bla bla...");
    return true;
};

LEGACY_BENCHMARK(IMB_sendrecv, sendrecv)
{
    descr.flags.insert(PARALLEL_TRANSFER);
    descr.flags.insert(SCALE_BW_DOUBLE);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
};

LEGACY_BENCHMARK(IMB_exchange, exchange)
{
    descr.flags.insert(PARALLEL_TRANSFER);
    descr.flags.insert(SCALE_BW_FOUR);
    descr.flags.insert(SENDBUF_SIZE_2I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
};

LEGACY_BENCHMARK(IMB_allreduce, allreduce)
{
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(REDUCTION);    
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
};

LEGACY_BENCHMARK(IMB_reduce, reduce)
{
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(REDUCTION);
    descr.flags.insert(HAS_ROOT);    
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
};

LEGACY_BENCHMARK(IMB_reduce_scatter, reduce_scatter)
{
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(REDUCTION);    
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
};

LEGACY_BENCHMARK(IMB_bcast, bcast)
{
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(HAS_ROOT);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
};

LEGACY_BENCHMARK(IMB_barrier, barrier)
{
    descr.flags.insert(SYNC);
    descr.flags.insert(SENDBUF_SIZE_0);
    descr.flags.insert(RECVBUF_SIZE_0);
    return true;
};

LEGACY_BENCHMARK(IMB_allgather, allgather)
{
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_NP_I);
    return true;
};

LEGACY_BENCHMARK(IMB_allgatherv, allgatherv)
{
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_NP_I);
    return true;
};

LEGACY_BENCHMARK(IMB_gather, gather)
{
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_NP_I);
    descr.flags.insert(HAS_ROOT);
    return true;
};

LEGACY_BENCHMARK(IMB_gatherv, gatherv)
{
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_NP_I);
    descr.flags.insert(HAS_ROOT);
    return true;
};

LEGACY_BENCHMARK(IMB_scatter, scatter)
{
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(SENDBUF_SIZE_NP_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    descr.flags.insert(HAS_ROOT);
    return true;
};

LEGACY_BENCHMARK(IMB_scatterv, scatterv)
{
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(SENDBUF_SIZE_NP_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    descr.flags.insert(HAS_ROOT);
    return true;
};

LEGACY_BENCHMARK(IMB_alltoall, alltoall)
{
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(SENDBUF_SIZE_NP_I);
    descr.flags.insert(RECVBUF_SIZE_NP_I);
    return true;
};

LEGACY_BENCHMARK(IMB_alltoallv, alltoallv)
{
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(SENDBUF_SIZE_NP_I);
    descr.flags.insert(RECVBUF_SIZE_NP_I);
    return true;
};

LEGACY_BENCHMARK(IMB_uni_bandwidth, uniband)
{
    descr.flags.insert(PARALLEL_TRANSFER_MSG_RATE);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
}

LEGACY_BENCHMARK(IMB_bi_bandwidth, biband)
{
    descr.flags.insert(PARALLEL_TRANSFER_MSG_RATE);
    descr.flags.insert(SCALE_BW_DOUBLE);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
}

#endif    
