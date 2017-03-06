#include <vector>
#include <string>
#include <set>
#include <map>
#include <assert.h>
#include "benchmark_suites_collection.h"

std::map<const std::string, BenchmarkSuiteBase*> *BenchmarkSuitesCollection::pnames = NULL;

struct DummyBenchmarkSuite : public BenchmarkSuiteBase {
    DummyBenchmarkSuite() { BenchmarkSuitesCollection::register_elem(this); }
    virtual const std::string get_name() const { return "__(dummy)__"; }
};

DummyBenchmarkSuite dummy_benchmark_suite;
