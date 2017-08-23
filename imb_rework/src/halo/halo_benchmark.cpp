#include <mpi.h>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <iostream>

#include "benchmark_suites_collection.h"
#include "utils.h"

#define WITH_VECTOR

#define GLUE_TYPENAME2(A,B) A,B
#define GLUE_TYPENAME3(A,B,C) A,B,C

#include "halo_benchmark.h"

using namespace std;

namespace ndim_halo_benchmark {
#include "benchmark_suite.h"    

DECLARE_INHERITED_TEMPLATE(HaloBenchmark<BenchmarkSuite<BS_GENERIC> >, simple_halo)
    
}
