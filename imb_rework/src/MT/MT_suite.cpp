#include <set>
#include <vector>
#include <string>
#include <map>
#include "benchmark.h"
#include "benchmark_suite.h"
#include "utils.h"

#include "MT_suite.h"

#include "immb.h"

static MPI_Comm immb_convert_comm(const char *str, int mode_multiple) {
    MPI_Comm comm = strcmp(str, "world") ? MPI_COMM_NULL : MPI_COMM_WORLD, new_comm;
    if(mode_multiple) {
        MPI_Info info;
        char ep_idx[128];
        sprintf(ep_idx, "%d", omp_get_thread_num());
        MPI_Info_create(&info);
        MPI_Info_set(info, "ep_idx", ep_idx);
#pragma omp ordered
        MPI_Comm_dup(comm, &new_comm);
        MPI_Comm_set_info(new_comm, info);
        MPI_Info_free(&info);
        return new_comm;
    }
    return comm;
}



using namespace std;

namespace NS_MT {
    immb_local_t input;
    int mode_multiple = 0;
    vector<int> cnt;
};

map<string, const Benchmark*, set_operations::case_insens_cmp> *BenchmarkSuite<BS_MT>::pnames = 0;
BenchmarkSuite<BS_MT> *BenchmarkSuite<BS_MT>::instance = 0;

template <> const std::string BenchmarkSuite<BS_MT>::get_name() const { return "IMB-MT"; }

template <> void BenchmarkSuite<BS_MT>::declare_args(args_parser &parser) const {
    parser.add_option_with_defaults<int>("warmup",  100);
    parser.add_option_with_defaults<int>("repeat", 1000);
    parser.add_option_with_defaults<int>("barrier", 1);
    parser.add_option_with_defaults_vec<string>("comm", "world");
    parser.add_option_with_defaults_vec<int>("count", "1,2,4,8");
//    immb_param_add(&input->user, "-pattern", "pt2pt");
}

template <> bool BenchmarkSuite<BS_MT>::prepare(const args_parser &parser, const set<string> &benchs) {
    using namespace NS_MT;
    input.global = (immb_global_t *)malloc(sizeof(immb_global_t));
    input.global->mode_multiple = mode_multiple;
    input.warmup = parser.get_result<int>("warmup");
    input.repeat = parser.get_result<int>("repeat");
    input.barrier = parser.get_result<int>("barrier");
    {
        vector<string> comm;
        parser.get_result_vec<string>("comm", comm);
        size_t n = comm.size();
        _ARRAY_ALLOC(input.comm, MPI_Comm, n);
        for (size_t i = 0; i < n; i++) {
            input.comm[i] = immb_convert_comm(comm[i].c_str(), mode_multiple);
        }
    }
    {
        parser.get_result_vec<int>("count", cnt);
        size_t n = cnt.size();
        _ARRAY_ALLOC(input.count, int, n);
        for (size_t i = 0; i < n; i++) {
            input.count[i] = cnt[i];
        }
    }
    return true;
}

template <> void BenchmarkSuite<BS_MT>::finalize(const set<string> &benchs) {
    using namespace NS_MT;
    _ARRAY_FREE(input.comm);
    _ARRAY_FREE(input.count);
}


void *MTBenchmarkSuite::get_internal_data_ptr(const std::string &key) {
    using namespace NS_MT;
    if (key == "input") return &input;
    return 0;
}

