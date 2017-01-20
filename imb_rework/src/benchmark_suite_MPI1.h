#include "benchmark_suite.h"


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

template <> bool BenchmarkSuite<BS_MPI1>::prepare(const std::string &yaml) {
    // INPUT: YAML-string of all given benchmark parameters, defaults are filled in
    // OUTPUT: c_info and other strictures for IMB cycle
    return true;
}


