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

#define LEGACY_BENCHMARK(LEGACY_BMRK_FN, LEGACY_BMRK_NAME) template class OriginalBenchmark<BenchmarkSuite<BS_EXT>, LEGACY_BMRK_FN>; \
DECLARE_INHERITED_TEMPLATE(GLUE_TYPENAME(OriginalBenchmark<BenchmarkSuite<BS_EXT>, LEGACY_BMRK_FN>), LEGACY_BMRK_NAME) \
template<> smart_ptr<reworked_Bmark_descr> OriginalBenchmark<BenchmarkSuite<BS_EXT>, LEGACY_BMRK_FN>::descr = NULL; \
template<> bool OriginalBenchmark<BenchmarkSuite<BS_EXT>, LEGACY_BMRK_FN>::init_description() 

LEGACY_BENCHMARK(IMB_window, Window)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(NO);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(NON_AGGREGATE);
    return true;
}

LEGACY_BENCHMARK(IMB_unidir_get, Unidir_Get)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(GET);
    return true;
}

LEGACY_BENCHMARK(IMB_unidir_put, Unidir_Put)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(PUT);
    return true;
}

LEGACY_BENCHMARK(IMB_bidir_get, Bidir_Get)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(GET);
    return true;
}

LEGACY_BENCHMARK(IMB_bidir_put, Bidir_Put)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(PUT);
    return true;
}

LEGACY_BENCHMARK(IMB_accumulate, Accumulate)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(REDUCTION);
    descr->flags.insert(PUT);
    return true;
}
