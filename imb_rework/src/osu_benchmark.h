#pragma once
#include "benchmark_suite.h"

using namespace std;

#define GLUE_TYPENAME(A,B) A,B

typedef int (*osu_benchmark_func_t)(int argc, char **argv);

template <class bs, osu_benchmark_func_t fn_ptr>
class OSUBenchmark : public Benchmark {
    static const char *name;
    public:
    virtual bool init_description() { return true; }
    virtual void init() {
    }
    virtual void run() { 
//        cout << "I am " << name << " benchmark wrapper and I've got benchmark function ptr: " << long(fn_ptr) << endl;
        char *argv = "";
        fn_ptr(1, &argv);
    }
    bool is_default() { return true; } 
    DEFINE_INHERITED(GLUE_TYPENAME(OSUBenchmark<bs, fn_ptr>), bs);
};

