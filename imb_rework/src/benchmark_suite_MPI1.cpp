#include "benchmark_suite_MPI1.h"
#include "IMB_comm_info.h"

namespace NS_MPI1 {
    struct comm_info c_info;
    struct iter_schedule ITERATIONS;
//    MODES BMODE;
//    double time_[MAX_TIME_ID];
//    int iter;
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

template <> bool BenchmarkSuite<BS_MPI1>::prepare(const std::string &yaml) {
    using namespace NS_MPI1;
    struct Bench *BList;
    IMB_set_default(&c_info);
    IMB_init_pointers(&c_info);
    char *argv[] = { "" };
    int argc = 0;
    int NPmin = 2;
    IMB_basic_input(&c_info, &BList, &ITERATIONS, &argc, (char ***)argv, &NPmin);
    // IMB_show_selections
//    Bmark->RUN_MODES[0].
//    IMB_valid(&c_info, Bmark, NP);

    // -- run
//    Bmark->name = strdup(name.c_str());
//    BMark->name = strdup("pingpong");
//    IMB_set_bmark(BMark);
//    MPI_Comm_size(MPI_COMM_WORLD, &NP);
//    IMB_init_communicator(&c_info, NP);
    // MPI_Type_size
//    iter = 0;
//    size = 1024;
//    IMB_init_buffers_iter(&c_info, &ITERATIONS, BMark, BMODE, iter, size);

//    IMB_warm_up(&c_info, BMark, &ITERATIONS, iter);
//    BMark->Benchmark(&c_info, size, &ITERATIONS, BMODE, time_);

    // INPUT: YAML-string of all given benchmark parameters, defaults are filled in
    // OUTPUT: c_info and other strictures for IMB cycle

    return true;
}

void *OriginalBenchmarkSuite_MPI1::get_internal_data_ptr(const std::string &key) {
    using namespace NS_MPI1;
    if (key == "c_info") return &c_info;
    if (key == "ITERATIONS") return &ITERATIONS;
    return 0;
}

