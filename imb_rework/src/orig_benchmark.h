#pragma once
#include "benchmark_suite.h"
#include "IMB_benchmark.h"

using namespace std;

#define GLUE_TYPENAME(A,B) A,B

typedef void (*original_benchmark_func_t)(struct comm_info* c_info, int size, 
        struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time);


template <benchmark_suite_t bs, original_benchmark_func_t fn_ptr>
class OriginalBenchmark : public Benchmark {
    static const char *name;
    public:
    virtual void run() { 
        cout << "I am " << name << " benchmark wrapper and I've got benchmark function ptr: " << long(fn_ptr) << endl;
        //fn_ptr(c_info, size, ITERATIONS, RUN_MODE, time_ptr);
    } 
    DEFINE_INHERITED(GLUE_TYPENAME(OriginalBenchmark<bs, fn_ptr>), bs);
};

