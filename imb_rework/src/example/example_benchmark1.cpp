#include <mpi.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>

#include "benchmark.h"
#include "benchmark_suites_collection.h"
#include "scope.h"
#include "utils.h"

using namespace std;

// NOTE: The following example use BS_GENERIC template of BenchmarkSuite.
// One needs to take care when using it:
// 1. Put benchmark_suite.h include directive and all its usage in the namespace
// with a unique name. It is required to avoid multiple occurence of static variables
// problem if BS_GENERIC is used by several suites as a base.
// 2. Put DECLARE_BENCHMARK_SUITE_STUFF right after include directive
// 3. Every benchmark you plan to put in suite must include DEFINE_INHERITED and
// DECLARE_INHERITED macros. 
// All other stuff is automated. Enjoy!
namespace example_suite1 {

    #include "benchmark_suite.h"
    DECLARE_BENCHMARK_SUITE_STUFF(BS_GENERIC, example_suite2)

    class ExampleBenchmark_2 : public Benchmark {
        public:
        virtual void run(const scope_item &item) { 
            UNUSED(item);
            cout << get_name() << ": Hello, world from example1" << endl;
        }
        DEFINE_INHERITED(ExampleBenchmark_2, BenchmarkSuite<BS_GENERIC>);
    };

    DECLARE_INHERITED(ExampleBenchmark_2, example2)
}
