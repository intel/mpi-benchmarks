#include "benchmark_suite.h"
#include "benchmark_suite_MPI1.h"

#ifdef MPI1
map<string, const Benchmark*> *BenchmarkSuite<BS_MPI1>::pnames = 0;
//map<string, const Benchmark*> *OriginalBenchmarkSuite_MPI1::pnames = 0;
#endif
map<string, const Benchmark*> *BenchmarkSuite<BS_OSU>::pnames = 0;
