#include <vector>
#include <string>
#include <set>
#include <map>
#include <assert.h>
#include "benchmark_suites_list.h"

std::map<const std::string, BenchmarkSuiteBase*> *BenchmarkSuitesCollection::pnames = NULL;
