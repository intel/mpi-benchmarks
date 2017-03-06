#include <vector>
#include <string>
#include <set>
#include <map>
#include <assert.h>
#include "benchmark_suite.h"
#include "benchmark_suites_collection.h"
#include "utils.h"

using namespace std;

map<const string, BenchmarkSuiteBase*> *BenchmarkSuitesCollection::pnames = NULL;

map<string, const Benchmark*, set_operations::case_insens_cmp> *BenchmarkSuite<BS_GENERIC>::pnames = 0;
BenchmarkSuite<BS_GENERIC> *BenchmarkSuite<BS_GENERIC>::instance = 0;

template <> const std::string BenchmarkSuite<BS_GENERIC>::get_name() const { return "__(generic)__"; }


struct DummyBenchmarkSuite : public BenchmarkSuite<BS_GENERIC> {
    DummyBenchmarkSuite() { BenchmarkSuitesCollection::register_elem(this); }
    const std::string get_name() const { return "__(dummy)__"; }
};

// dummy suite for the case there is no other suites registered
namespace { DummyBenchmarkSuite dummy_benchmark_suite; }


