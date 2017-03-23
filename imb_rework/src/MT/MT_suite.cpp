#include <set>
#include <vector>
#include <string>
#include <map>
#include "benchmark.h"
#include "benchmark_suite.h"
#include "utils.h"

#include "MT_suite.h"

#include "immb.h"

static MPI_Comm immb_convert_comm(const char *str, int mode_multiple, int thread_num) {
    MPI_Comm comm = strcmp(str, "world") ? MPI_COMM_NULL : MPI_COMM_WORLD, new_comm;
    if(mode_multiple) {
        MPI_Info info;
        char ep_idx[128];
        sprintf(ep_idx, "%d", thread_num);
        MPI_Info_create(&info);
        MPI_Info_set(info, "ep_idx", ep_idx);
        MPI_Comm_dup(comm, &new_comm);
        MPI_Comm_set_info(new_comm, info);
        MPI_Info_free(&info);
        return new_comm;
    }
    return comm;
}



using namespace std;

namespace NS_MT {
    immb_local_t *input;
    int mode_multiple;
    int stride;
    int num_threads;
    vector<int> cnt;
    vector<string> comm_opts;
};

map<string, const Benchmark*, set_operations::case_insens_cmp> *BenchmarkSuite<BS_MT>::pnames = 0;
BenchmarkSuite<BS_MT> *BenchmarkSuite<BS_MT>::instance = 0;

template <> const std::string BenchmarkSuite<BS_MT>::get_name() const { return "IMB-MT"; }

template <> void BenchmarkSuite<BS_MT>::declare_args(args_parser &parser) const {
    parser.add_option_with_defaults<int>("stride", 0);
    parser.add_option_with_defaults<int>("warmup",  100);
    parser.add_option_with_defaults<int>("repeat", 1000);
    parser.add_option_with_defaults<bool>("barrier", true);
    parser.add_option_with_defaults_vec<string>("comm", "world");
    parser.add_option_with_defaults_vec<int>("count", "1,2,4,8").
        set_mode(args_parser::option::APPLY_DEFAULTS_ONLY_WHEN_MISSING);
}

template <> bool BenchmarkSuite<BS_MT>::prepare(const args_parser &parser, const set<string> &benchs) {
    using namespace NS_MT;
    parser.get_result_vec<string>("comm", comm_opts);
    parser.get_result_vec<int>("count", cnt);
    mode_multiple = (parser.get_result<string>("thread_level") == "multiple");
    stride = parser.get_result<int>("stride");
    num_threads = 1;
    if (mode_multiple) {
#pragma omp parallel default(shared)
#pragma omp master        
        num_threads = omp_get_num_threads();
    } 
    input = (immb_local_t *)malloc(sizeof(immb_local_t) * num_threads);
    for (int thread_num = 0; thread_num < num_threads; thread_num++) {
        {
            size_t n = comm_opts.size();
            _ARRAY_ALLOC(input[thread_num].comm, MPI_Comm, n);
            for (size_t i = 0; i < n; i++) {
                input[thread_num].comm[i] = immb_convert_comm(comm_opts[i].c_str(), mode_multiple, thread_num);
            }
        }
        {
            size_t n = cnt.size();
            _ARRAY_ALLOC(input[thread_num].count, int, n);
            for (size_t i = 0; i < n; i++) {
                input[thread_num].count[i] = cnt[i];
            }
        }
        input[thread_num].global = (immb_global_t *)malloc(sizeof(immb_global_t));
        input[thread_num].global->mode_multiple = mode_multiple;
        input[thread_num].warmup = parser.get_result<int>("warmup");
        input[thread_num].repeat = parser.get_result<int>("repeat");
        input[thread_num].barrier = parser.get_result<int>("barrier");
    }
    return true;
}

template <> void BenchmarkSuite<BS_MT>::finalize(const set<string> &benchs) {
    using namespace NS_MT;
    for (int thread_num = 0; thread_num < num_threads; thread_num++) {
        _ARRAY_FREE(input[thread_num].comm);
        _ARRAY_FREE(input[thread_num].count);
    }
}


void *MTBenchmarkSuite::get_internal_data_ptr(const std::string &key, int i) {
    using namespace NS_MT;
    if (key == "input[thread_num]") return &input[i];
    if (key == "num_threads") return &num_threads;
    if (key == "mode_multiple") return &mode_multiple;
    if (key == "stride") return &stride;
    assert(false);
    return 0;
}

