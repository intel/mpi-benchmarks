#pragma once

template <> void BenchmarkSuite<BS_MT>::declare_args(args_parser &parser) const;

template <> bool BenchmarkSuite<BS_MT>::prepare(const args_parser &parser, const std::set<std::string> &benchs);

template <> void BenchmarkSuite<BS_MT>::finalize(const std::set<std::string> &benchs);

// FIXME code duplication

/* convenience macros and functions working with input parameters */
#define _ARRAY_DECL(_array,_type) _type *_array; int _array##n; int _array##i
#define _ARRAY_ALLOC(_array,_type,_size) _array##i=0; _array=(_type*)malloc((_array##n=_size)*sizeof(_type))
#define _ARRAY_FREE(_array) free(_array)
#define _ARRAY_FOR(_array,_i,_action) for(_i=0;_i<_array##n;_i++) {_action;}
#define _ARRAY_NEXT(_array) if(++_array##i >= _array##n) _array##i = 0; else return 1
#define _ARRAY_THIS(_array) (_array[_array##i])
#define _ARRAY_CHECK(_array) (_array##i >= _array##n)

typedef struct _immb_local_t {
    int warmup;
    int repeat;
    _ARRAY_DECL(comm, MPI_Comm);
    _ARRAY_DECL(count, int);
} immb_local_t;

enum malopt_t {
    MALOPT_SERIAL,
    MALOPT_CONTINOUS,
    MALOPT_PARALLEL
};

enum barropt_t {
    BARROPT_NOBARRIER,
    BARROPT_NORMAL,
    BARROPT_SPECIAL
};

class MTBenchmarkSuite : public BenchmarkSuite<BS_MT> {
    public:
        using BenchmarkSuite<BS_MT>::prepare;
        using BenchmarkSuite<BS_MT>::declare_args;
        using BenchmarkSuite<BS_MT>::create;
        using BenchmarkSuite<BS_MT>::register_elem;
        static void *get_internal_data_ptr(const std::string &key, int i = 0);
};

