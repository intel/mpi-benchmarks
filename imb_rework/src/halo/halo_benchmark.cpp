#include <mpi.h>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <iostream>

#include "benchmark_suites_collection.h"
#include "utils.h"
#include "halo_suite.h"

#define WITH_VECTOR

#define GLUE_TYPENAME2(A,B) A,B
#define GLUE_TYPENAME3(A,B,C) A,B,C

#include "halo_benchmark.h"

using namespace std;

namespace ndim_halo_benchmark {

DECLARE_INHERITED_TEMPLATE(HaloBenchmark<HALOBenchmarkSuite>, simple_halo)
    
}
