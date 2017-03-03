#pragma once
#include "benchmark_suite.h"

#define GLUE_TYPENAME(A,B) A,B

typedef int (*osu_benchmark_func_t)(int argc, char **argv);

template <class bs, osu_benchmark_func_t fn_ptr>
class OSUBenchmark : public Benchmark {
    static const char *name;
    public:
    virtual bool init_description() { return true; }
    virtual void init() { }
    virtual void run(const std::pair<int, int> &) { 
        char *argv = "";
        fn_ptr(1, &argv);
    }
    bool is_default() { return true; } 
    OSUBenchmark<bs, fn_ptr>() { bs::register_elem(this); }
    DEFINE_INHERITED(GLUE_TYPENAME(OSUBenchmark<bs, fn_ptr>), bs);
};

