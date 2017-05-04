#ifdef MPI1

#include <mpi.h>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <iostream>
#include "legacy_benchmark.h"

extern "C" {
#include "IMB_prototypes.h"
}

#include "reworked_IMB_functions.h"

using namespace std;

#define LEGACY_BENCHMARK(LEGACY_BMRK_FN, LEGACY_BMRK_NAME) template class OriginalBenchmark<OriginalBenchmarkSuite_MPI1, LEGACY_BMRK_FN>; \
DECLARE_INHERITED_TEMPLATE(GLUE_TYPENAME(OriginalBenchmark<OriginalBenchmarkSuite_MPI1, LEGACY_BMRK_FN>), LEGACY_BMRK_NAME) \
template<> reworked_Bmark_descr OriginalBenchmark<OriginalBenchmarkSuite_MPI1, LEGACY_BMRK_FN>::descr  = {}; \
template<> bool OriginalBenchmark<OriginalBenchmarkSuite_MPI1, LEGACY_BMRK_FN>::init_description() 

LEGACY_BENCHMARK(IMB_pingpong, PingPong)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(SELECT_SOURCE);
    descr.flags.insert(SINGLE_TRANSFER);
    descr.flags.insert(SCALE_TIME_HALF);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    descr.comments.push_back("This is a pingpong benchmark bla bla bla...");
    return true;
};

void IMB_pingpong_specificsource(struct comm_info* c_info, int size,
                                 struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_pingpong(c_info, size, ITERATIONS, RUN_MODE, time);
}

LEGACY_BENCHMARK(IMB_pingpong_specificsource, PingPongSpecificSource)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(SELECT_SOURCE);
    descr.flags.insert(SINGLE_TRANSFER);
    descr.flags.insert(SCALE_TIME_HALF);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
};

void IMB_pingpong_anysource(struct comm_info* c_info, int size,
                            struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_pingpong(c_info, size, ITERATIONS, RUN_MODE, time);
}

LEGACY_BENCHMARK(IMB_pingpong_anysource, PingPongAnySource)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(SINGLE_TRANSFER);
    descr.flags.insert(SCALE_TIME_HALF);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
};

LEGACY_BENCHMARK(IMB_pingping, PingPing)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(SELECT_SOURCE);
    descr.flags.insert(SINGLE_TRANSFER);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
};

void IMB_pingping_specificsource(struct comm_info* c_info, int size,
                                 struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_pingping(c_info, size, ITERATIONS, RUN_MODE, time);
}

LEGACY_BENCHMARK(IMB_pingping_specificsource, PingPingSpecificSource)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(SELECT_SOURCE);
    descr.flags.insert(SINGLE_TRANSFER);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
};

void IMB_pingping_anysource(struct comm_info* c_info, int size,
                            struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_pingping(c_info, size, ITERATIONS, RUN_MODE, time);
}

LEGACY_BENCHMARK(IMB_pingping_anysource, PingPingAnySource)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(SINGLE_TRANSFER);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
};

LEGACY_BENCHMARK(IMB_sendrecv, Sendrecv)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(PARALLEL_TRANSFER);
    descr.flags.insert(SCALE_BW_DOUBLE);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
};

LEGACY_BENCHMARK(IMB_exchange, Exchange)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(PARALLEL_TRANSFER);
    descr.flags.insert(SCALE_BW_FOUR);
    descr.flags.insert(SENDBUF_SIZE_2I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
};

LEGACY_BENCHMARK(IMB_allreduce, Allreduce)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(REDUCTION);    
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
};

LEGACY_BENCHMARK(IMB_reduce, Reduce)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(REDUCTION);
    descr.flags.insert(HAS_ROOT);    
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
};

LEGACY_BENCHMARK(IMB_reduce_scatter, Reduce_scatter)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(REDUCTION);    
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
};

LEGACY_BENCHMARK(IMB_bcast, Bcast)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(HAS_ROOT);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
};

LEGACY_BENCHMARK(IMB_barrier, Barrier)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(SYNC);
    descr.flags.insert(SENDBUF_SIZE_0);
    descr.flags.insert(RECVBUF_SIZE_0);
    return true;
};

LEGACY_BENCHMARK(IMB_allgather, Allgather)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_NP_I);
    return true;
};

LEGACY_BENCHMARK(IMB_allgatherv, Allgatherv)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_NP_I);
    return true;
};

//template<> reworked_Bmark_descr OriginalBenchmark<OriginalBenchmarkSuite_MPI1, IMB_gather>::descr = {};

LEGACY_BENCHMARK(IMB_gather, Gather)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_NP_I);
    descr.flags.insert(HAS_ROOT);
    return true;
};

LEGACY_BENCHMARK(IMB_gatherv, Gatherv)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_NP_I);
    descr.flags.insert(HAS_ROOT);
    return true;
};

LEGACY_BENCHMARK(IMB_scatter, Scatter)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(SENDBUF_SIZE_NP_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    descr.flags.insert(HAS_ROOT);
    return true;
};

LEGACY_BENCHMARK(IMB_scatterv, Scatterv)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(SENDBUF_SIZE_NP_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    descr.flags.insert(HAS_ROOT);
    return true;
};

LEGACY_BENCHMARK(IMB_alltoall, Alltoall)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(SENDBUF_SIZE_NP_I);
    descr.flags.insert(RECVBUF_SIZE_NP_I);
    return true;
};

LEGACY_BENCHMARK(IMB_alltoallv, Alltoallv)
{
    descr.flags.insert(DEFAULT);
    descr.flags.insert(COLLECTIVE);
    descr.flags.insert(SENDBUF_SIZE_NP_I);
    descr.flags.insert(RECVBUF_SIZE_NP_I);
    return true;
};

LEGACY_BENCHMARK(IMB_uni_bandwidth, Uniband)
{
    descr.flags.insert(PARALLEL_TRANSFER_MSG_RATE);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
}

LEGACY_BENCHMARK(IMB_bi_bandwidth, Biband)
{
    descr.flags.insert(PARALLEL_TRANSFER_MSG_RATE);
    descr.flags.insert(SCALE_BW_DOUBLE);
    descr.flags.insert(SENDBUF_SIZE_I);
    descr.flags.insert(RECVBUF_SIZE_I);
    return true;
}

#endif    
