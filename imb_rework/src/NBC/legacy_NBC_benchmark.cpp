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
#include "legacy_benchmark.h"

extern "C" {
#include "IMB_benchmark.h"
#include "IMB_comm_info.h"
#include "IMB_prototypes.h"
}

#include "reworked_IMB_functions.h"

using namespace std;

#define LEGACY_BENCHMARK(LEGACY_BMRK_FN, LEGACY_BMRK_NAME) template class OriginalBenchmark<BenchmarkSuite<BS_NBC>, LEGACY_BMRK_FN>; \
DECLARE_INHERITED_TEMPLATE(GLUE_TYPENAME(OriginalBenchmark<BenchmarkSuite<BS_NBC>, LEGACY_BMRK_FN>), LEGACY_BMRK_NAME) \
template<> smart_ptr<reworked_Bmark_descr> OriginalBenchmark<BenchmarkSuite<BS_NBC>, LEGACY_BMRK_FN>::descr = NULL; \
template<> bool OriginalBenchmark<BenchmarkSuite<BS_NBC>, LEGACY_BMRK_FN>::init_description() 

LEGACY_BENCHMARK(IMB_ibcast, Ibcast)
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

LEGACY_BENCHMARK(IMB_ibcast_pure, Ibcast_pure)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(COLLECTIVE);
    return true;
}

LEGACY_BENCHMARK(IMB_iallgather, Iallgather)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(COLLECTIVE);
    return true;
}

LEGACY_BENCHMARK(IMB_iallgather_pure, Iallgather_pure)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(COLLECTIVE);
    return true;
}

LEGACY_BENCHMARK(IMB_iallgatherv, Iallgatherv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(COLLECTIVE);
    return true;
}

LEGACY_BENCHMARK(IMB_iallgatherv_pure, Iallgatherv_pure)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(COLLECTIVE);
    return true;
}

LEGACY_BENCHMARK(IMB_igather, Igather)
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

LEGACY_BENCHMARK(IMB_igather_pure, Igather_pure)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(COLLECTIVE);
    return true;
}

LEGACY_BENCHMARK(IMB_igatherv, Igatherv)
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

LEGACY_BENCHMARK(IMB_igatherv_pure, Igatherv_pure)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(COLLECTIVE);
    return true;
}

LEGACY_BENCHMARK(IMB_iscatter, Iscatter)
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

LEGACY_BENCHMARK(IMB_iscatter_pure, Iscatter_pure)
{
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(COLLECTIVE);
    return true;
}

LEGACY_BENCHMARK(IMB_iscatterv, Iscatterv)
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

LEGACY_BENCHMARK(IMB_iscatterv_pure, Iscatterv_pure)
{
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(HAS_ROOT);
    descr->flags.insert(COLLECTIVE);
    return true;
}

LEGACY_BENCHMARK(IMB_ialltoall, Ialltoall)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(COLLECTIVE);
    return true;
}

LEGACY_BENCHMARK(IMB_ialltoall_pure, Ialltoall_pure)
{
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(COLLECTIVE);
    return true;
}

LEGACY_BENCHMARK(IMB_ialltoallv, Ialltoallv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(COLLECTIVE);
    return true;
}

LEGACY_BENCHMARK(IMB_ialltoallv_pure, Ialltoallv_pure)
{
    descr->flags.insert(SENDBUF_SIZE_NP_I);
    descr->flags.insert(RECVBUF_SIZE_NP_I);
    descr->flags.insert(COLLECTIVE);
    return true;
}

LEGACY_BENCHMARK(IMB_ireduce, Ireduce)
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

LEGACY_BENCHMARK(IMB_ireduce_pure, Ireduce_pure)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(COLLECTIVE);
    return true;
}

LEGACY_BENCHMARK(IMB_ireduce_scatter, Ireduce_scatter)
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

LEGACY_BENCHMARK(IMB_ireduce_scatter_pure, Ireduce_scatter_pure)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(COLLECTIVE);
    return true;
}

LEGACY_BENCHMARK(IMB_iallreduce, Iallreduce)
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

LEGACY_BENCHMARK(IMB_iallreduce_pure, Iallreduce_pure)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(COLLECTIVE);
    return true;
}

LEGACY_BENCHMARK(IMB_ibarrier, Ibarrier)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_3);
    descr->flags.insert(SYNC);
    return true;
}

LEGACY_BENCHMARK(IMB_ibarrier_pure, Ibarrier_pure)
{
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SYNC);
    return true;
}
