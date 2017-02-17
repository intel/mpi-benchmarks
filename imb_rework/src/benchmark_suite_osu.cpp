#include <set>
#include <vector>
#include <string>
#include <map>
#include "benchmark_suite.h"

using namespace std;

map<string, const Benchmark*, set_operations::case_insens_cmp> *BenchmarkSuite<BS_OSU>::pnames = 0;
BenchmarkSuite<BS_OSU> *BenchmarkSuite<BS_OSU>::instance = 0;

template <> const std::string BenchmarkSuite<BS_OSU>::get_name() const { return "OSU"; }
template <> void BenchmarkSuite<BS_OSU>::declare_args(args_parser&) const {}

template <> bool BenchmarkSuite<BS_OSU>::prepare(args_parser const&, const std::set<std::string> &) { return true; }

template <> void BenchmarkSuite<BS_OSU>::get_bench_list(std::set<std::string> &list, BenchmarkSuiteBase::BenchListFilter filter) const { get_full_list(list); }

