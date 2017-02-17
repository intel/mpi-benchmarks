#if 0
#include <mpi.h>
#include <vector>
#include <set>
#include <map>
#include "benchmark_suite.h"
#include "benchmark_suite_MPI1.h"

using namespace std;
#ifdef MPI1
map<string, const Benchmark*> *BenchmarkSuite<BS_MPI1>::pnames = 0;
BenchmarkSuite<BS_MPI1> *BenchmarkSuite<BS_MPI1>::instance = 0;
//map<string, const Benchmark*> *OriginalBenchmarkSuite_MPI1::pnames = 0;
#endif
map<string, const Benchmark*> *BenchmarkSuite<BS_OSU>::pnames = 0;
BenchmarkSuite<BS_OSU> *BenchmarkSuite<BS_OSU>::instance = 0;
#endif
