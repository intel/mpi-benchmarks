#pragma once

/*
// convenience macros and functions working with input parameters
#define _ARRAY_DECL(_array,_type) _type *_array; int _array##n; int _array##i
#define _ARRAY_ALLOC(_array,_type,_size) _array##i=0; _array=(_type*)malloc((_array##n=_size)*sizeof(_type))
#define _ARRAY_FREE(_array) free(_array)
#define _ARRAY_FOR(_array,_i,_action) for(_i=0;_i<_array##n;_i++) {_action;}
#define _ARRAY_NEXT(_array) if(++_array##i >= _array##n) _array##i = 0; else return 1
#define _ARRAY_THIS(_array) (_array[_array##i])
#define _ARRAY_CHECK(_array) (_array##i >= _array##n)
*/


struct thread_local_data_t {
    int warmup;
    int repeat;
    MPI_Comm comm;
//    std::vector<int> count;
};

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

