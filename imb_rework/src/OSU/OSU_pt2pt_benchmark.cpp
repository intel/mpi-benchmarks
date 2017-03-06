#ifdef OSU

#include <mpi.h>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <iostream>
#include "OSU_benchmark.h"

using namespace std;

#include "osu/mpi/pt2pt/osu_pt2pt.c"
#define main osu_bw
#define MPI_Init(a, b) ;
#define MPI_Finalize() ;
#include "osu/mpi/pt2pt/osu_bw.c"
template class OSUBenchmark<BenchmarkSuite<BS_OSU>, osu_bw>;
DECLARE_INHERITED(GLUE_TYPENAME(OSUBenchmark<BenchmarkSuite<BS_OSU>, osu_bw>), osu_bw)

#endif
