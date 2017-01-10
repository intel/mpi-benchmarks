#include "benchmark_suite.h"


template <> void BenchmarkSuite<BS_MPI1>::declare_args(args_parser &parser) {
    parser.add_option_with_defaults("npmin", args_parser::INT, args_parser::value(2)).
        set_caption("npmin", "NPmin");
    parser.add_option_with_defaults("multi", args_parser::INT, args_parser::value(0)).
        set_caption("multi", "MultiMode");
    parser.add_option_with_defaults_vec("off_cache", args_parser::INT, "-1", ',', 1, 2).
        set_caption("off_cache", "cache_size[,cache_line_size]");
    parser.add_option_with_defaults_vec("iter", args_parser::INT, "1000,40,100", ',', 1, 3).
        set_caption("iter", "msgspersample[,overall_vol[,msgs_nonaggr]]");
    parser.add_option_with_defaults("iter_policy", args_parser::STRING, args_parser::value("dynamic")).
        set_caption("iter_policy", "iter_policy");
    parser.add_option_with_defaults("time", args_parser::FLOAT, args_parser::value(10.0f)).
        set_caption("time", "max_runtime per sample");
    parser.add_option_with_defaults("mem", args_parser::FLOAT, args_parser::value(1.0f)).
        set_caption("mem", "max. per process memory for overall message buffers");
    parser.add_option_with_defaults("msglen", args_parser::STRING, args_parser::value("")).
        set_caption("msglen", "Lengths_file");
    parser.add_option_with_defaults_vec("map", args_parser::INT, "1x1", 'x', 2, 2).
        set_caption("map", "PxQ");
    parser.add_option_with_defaults_vec("msglog", args_parser::INT, "0:22", ':', 2, 2).
        set_caption("msglog", "min_msglog:max_msglog");
    parser.add_option_with_defaults("root_shift", args_parser::BOOL, args_parser::value(false)).
        set_caption("root_shift", "on or off");
    parser.add_option_with_defaults("sync", args_parser::BOOL, args_parser::value(true)).
        set_caption("sync", "on or off");
    parser.add_option_with_defaults("imb_barrier", args_parser::BOOL, args_parser::value(false)).
        set_caption("imb_barrier", "on or off");
}

template <> bool BenchmarkSuite<BS_MPI1>::prepare(const std::string &yaml) {
    // INPUT: YAML-string of all given benchmark parameters, defaults are filled in
    // OUTPUT: c_info and other strictures for IMB cycle
    return true;
}


