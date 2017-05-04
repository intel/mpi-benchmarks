#include <set>
#include <vector>
#include <string>
#include <map>
#include "benchmark.h"
#include "benchmark_suite.h"
#include "utils.h"

using namespace std;

template<> map<string, const Benchmark*, set_operations::case_insens_cmp> *BenchmarkSuite<BS_EXAMPLE>::pnames = 0;
template<> BenchmarkSuite<BS_EXAMPLE> *BenchmarkSuite<BS_EXAMPLE>::instance = 0;

template <> const std::string BenchmarkSuite<BS_EXAMPLE>::get_name() const { return "EXAMPLE"; }
