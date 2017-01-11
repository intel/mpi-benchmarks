#pragma once
#include "benchmark_suite.h"

using namespace std;

#define GLUE_TYPENAME(A,B) A,B

typedef int (*osu_benchmark_func_t)(int argc, char **argv);

template <benchmark_suite_t bs, osu_benchmark_func_t fn_ptr>
class OSUBenchmark : public Benchmark {
    static const char *name;
    public:
    virtual void run() { 
//        cout << "I am " << name << " benchmark wrapper and I've got benchmark function ptr: " << long(fn_ptr) << endl;
        char *argv = "";
        fn_ptr(1, &argv);
    } 
    DEFINE_INHERITED(GLUE_TYPENAME(OSUBenchmark<bs, fn_ptr>), bs);
};

