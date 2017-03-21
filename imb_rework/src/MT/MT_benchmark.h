#pragma once
#include <mpi.h>
#include <iostream>

#include "benchmark.h"
#include "benchmark_suite.h"

#include "immb.h"
#include "immb_pattern.c"

using namespace std;

#define GET_GLOBAL(TYPE, NAME) { TYPE *p = (TYPE *)bs::get_internal_data_ptr(#NAME); memcpy(&NAME, p, sizeof(TYPE)); }

template<class bs>
class PingPongMT : public Benchmark {
    static const char *name;
    char *a, *b;
    immb_local_t input;
    int mode_multiple;
    public:
    virtual void init() {
        // transfer input from suite
        GET_GLOBAL(immb_local_t, input);
        // transfer multiple from suite
        mode_multiple = input.global->mode_multiple;

        // add pattern
        _ARRAY_ALLOC(input.pattern, char *, 1);
        _ARRAY_THIS(input.pattern) = strdup("pt2pt");

        VarLenScope *sc = new VarLenScope(input.count, input.countn);
        scope = sc;

        // get longest element from sequence
        size_t maxlen = sc->get_max_len(); 

        // allocate a and b
        a = (char*)malloc(sizeof(char)*maxlen);
        b = (char*)malloc(sizeof(char)*maxlen);
    }
    virtual void run(const std::pair<int, int> &p) { 
        cout << "Hello, world! " << p.second << endl;
        if (mode_multiple) {
        #pragma omp parallel default(shared)
            immb_run_instance(&input, a, b, p.second);
        } else {
            immb_run_instance(&input, a, b, p.second);
        }
    }
    virtual void finalize() {
        free(_ARRAY_THIS(input.pattern));
        free(a);
        free(b);
    }
    DEFINE_INHERITED(PingPongMT<bs>, bs);
};

