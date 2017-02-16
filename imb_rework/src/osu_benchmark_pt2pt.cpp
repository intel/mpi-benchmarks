#include <mpi.h>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <iostream>
#include "osu_benchmark.h"

using namespace std;

template <> const std::string BenchmarkSuite<BS_OSU>::get_name() const { return "OSU"; }
template <> void BenchmarkSuite<BS_OSU>::declare_args(args_parser&) const {}

template <> bool BenchmarkSuite<BS_OSU>::prepare(args_parser const&, const std::set<std::string> &) { return true; }

template <> void BenchmarkSuite<BS_OSU>::get_bench_list(std::set<std::string> &list, BenchmarkSuiteBase::BenchListFilter filter) const { get_full_list(list); }


#include "osu/mpi/pt2pt/osu_pt2pt.c"
#define main osu_bw
#define MPI_Init(a, b) ;
#define MPI_Finalize() ;
#include "osu/mpi/pt2pt/osu_bw.c"
template class OSUBenchmark<BenchmarkSuite<BS_OSU>, osu_bw>;
DECLARE_INHERITED(GLUE_TYPENAME(OSUBenchmark<BenchmarkSuite<BS_OSU>, osu_bw>), osu_bw)

