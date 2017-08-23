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

namespace example_suite2 {

    #include "benchmark_suite.h"
    DECLARE_BENCHMARK_SUITE_STUFF(BS_GENERIC, example_suite2)

    // EXAMPLE 2: benchmark scope is added by overloading init() virtal function
    class ExampleBenchmark_2 : public Benchmark {
        public:
        virtual void init() {
            VarLenScope *sc = new VarLenScope(0, 22);
            scope = sc;
        }
        virtual void run(const scope_item &item) { 
            cout << get_name() << ": Hello, world! size=" << item.len << endl;
        }
        DEFINE_INHERITED(ExampleBenchmark_2, BenchmarkSuite<BS_GENERIC>);
    };

    DECLARE_INHERITED(ExampleBenchmark_2, example2)

}
