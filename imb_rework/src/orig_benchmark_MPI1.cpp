#ifdef MPI1

#include <iostream>
#include "orig_benchmark.h"

using namespace std;

#if 0
void func_one(int x) { cout << x << endl; }
void func_two(int x) { cout << x*x << endl; }

template class OriginalBenchmark<BS_MPI1, func_one>;
DECLARE_INHERITED(GLUE_TYPENAME(OriginalBenchmark<BS_MPI1, func_one>), func_one)

template class OriginalBenchmark<BS_MPI1, func_two>;
DECLARE_INHERITED(GLUE_TYPENAME(OriginalBenchmark<BS_MPI1, func_two>), func_two)
#endif

// void IMB_pingpong(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
//                   MODES RUN_MODE, double* time)

#include "original_imb/src/IMB_pingpong.c"
template class OriginalBenchmark<BS_MPI1, IMB_pingpong>;
DECLARE_INHERITED(GLUE_TYPENAME(OriginalBenchmark<BS_MPI1, IMB_pingpong>), pingpong)

#include "original_imb/src/IMB_pingping.c"
template class OriginalBenchmark<BS_MPI1, IMB_pingping>;
DECLARE_INHERITED(GLUE_TYPENAME(OriginalBenchmark<BS_MPI1, IMB_pingping>), pingping)



#endif    
