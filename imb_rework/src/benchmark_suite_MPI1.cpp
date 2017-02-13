#include <set>
#include <vector>
#include <string>
#include <algorithm>
#include <mpi.h>
#include "benchmark_suite_MPI1.h"
#include "IMB_comm_info.h"

#include "reworked_IMB_functions.h"

namespace NS_MPI1 {
    struct comm_info c_info;
    struct iter_schedule ITERATIONS;
    struct LEGACY_GLOBALS glob;
}


template <> void BenchmarkSuite<BS_MPI1>::init() {
    vector<string> benchs;
    BenchmarkSuite<BS_MPI1>::get_full_list(benchs);
    for (vector<string>::iterator it = benchs.begin(); it != benchs.end(); ++it) {
        smart_ptr<Benchmark> b = BenchmarkSuite<BS_MPI1>::create(*it);
        if (!b->init_description())
            throw logic_error("BenchmarkSuite<BS_MPI1>: wrong description of one of benchmarks in suite");
    }
}

template <> void BenchmarkSuite<BS_MPI1>::declare_args(args_parser &parser) {
    parser.add_option_with_defaults<int>("npmin", 2).
        set_caption("npmin", "NPmin");
    parser.add_option_with_defaults<int>("multi", 0).
        set_caption("multi", "MultiMode");
    parser.add_option_with_defaults_vec<int>("off_cache", "-1", ',', 1, 2).
        set_caption("off_cache", "cache_size[,cache_line_size]");
    parser.add_option_with_defaults_vec<int>("iter", "1000,40,100", ',', 1, 3).
        set_caption("iter", "msgspersample[,overall_vol[,msgs_nonaggr]]");
    parser.add_option_with_defaults<string>("iter_policy", "dynamic").
        set_caption("iter_policy", "iter_policy");
    parser.add_option_with_defaults<float>("time", 10.0f).
        set_caption("time", "max_runtime per sample");
    parser.add_option_with_defaults<float>("mem", 1.0f).
        set_caption("mem", "max. per process memory for overall message buffers");
    parser.add_option_with_defaults<string>("msglen", "").
        set_caption("msglen", "Lengths_file");
    parser.add_option_with_defaults_vec<int>("map", "1x1", 'x', 2, 2).
        set_caption("map", "PxQ");
    parser.add_option_with_defaults_vec<int>("msglog", "0:22", ':', 2, 2).
        set_caption("msglog", "min_msglog:max_msglog");
    parser.add_option_with_defaults<bool>("root_shift", false).
        set_caption("root_shift", "on or off");
    parser.add_option_with_defaults<bool>("sync", true).
        set_caption("sync", "on or off");
    parser.add_option_with_defaults<bool>("imb_barrier", false).
        set_caption("imb_barrier", "on or off");
}

template <> bool BenchmarkSuite<BS_MPI1>::prepare(const args_parser &parser, set<string> &benchs) {
    using namespace NS_MPI1;
    set<string> all_benchs, intersection;
    BenchmarkSuite<BS_MPI1>::get_full_list(all_benchs);
    set_intersection(all_benchs.begin(), all_benchs.end(), benchs.begin(), benchs.end(),
                     inserter(intersection, intersection.begin()));
    if (intersection.size() == 0)
        return true;
    struct Bench *BList;
    IMB_set_default(&c_info);
    IMB_init_pointers(&c_info);
    char *argv[] = { "" };
    int argc = 0;
    IMB_basic_input(&c_info, &BList, &ITERATIONS, &argc, (char ***)argv, &glob.NP_min);
    if (c_info.w_rank == 0 ) {
        IMB_general_info();
        fprintf(unit,"\n\n# Calling sequence was: \n\n");
        fprintf(unit,"# ------------------- \n\n");
        if (c_info.n_lens) {
            fprintf(unit,"# Message lengths were user defined\n");
        } else {
            fprintf(unit,"# Minimum message length in bytes:   %d\n",0);
            fprintf(unit,"# Maximum message length in bytes:   %d\n", 1<<c_info.max_msg_log);
        }

        fprintf(unit,"#\n");
        fprintf(unit,"# MPI_Datatype                   :   MPI_BYTE \n");
        fprintf(unit,"# MPI_Datatype for reductions    :   MPI_FLOAT\n");
        fprintf(unit,"# MPI_Op                         :   MPI_SUM  \n");
        fprintf(unit,"# \n");
        fprintf(unit,"# \n");
        fprintf(unit,"\n");
        fprintf(unit,"# List of Benchmarks to run:\n\n");
        for (set<string>::iterator it = intersection.begin(); it != intersection.end(); ++it) {
            printf("# %s\n", it->c_str());
        }
    }
    return true;
}

void OriginalBenchmarkSuite_MPI1::get_default_list(vector<string> &default_benchmarks) {
    BenchmarkSuite<BS_MPI1>::get_full_list(default_benchmarks);
    for (vector<string>::iterator it = default_benchmarks.begin(); it != default_benchmarks.end();) {
        smart_ptr<Benchmark> b = BenchmarkSuite<BS_MPI1>::create(*it);
        if (!b->is_default()) {
            it = default_benchmarks.erase(it);
        }
        else
            ++it;
    }
}

void *OriginalBenchmarkSuite_MPI1::get_internal_data_ptr(const std::string &key) {
    using namespace NS_MPI1;
    if (key == "c_info") return &c_info;
    if (key == "ITERATIONS") return &ITERATIONS;
    if (key == "glob") return &glob;
    return 0;
}

