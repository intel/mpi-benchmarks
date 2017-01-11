#include <iostream>
#include "osu_benchmark.h"

using namespace std;

#include "osu/mpi/pt2pt/osu_pt2pt.c"
#define main osu_bw
#include "osu/mpi/pt2pt/osu_bw.c"
template class OSUBenchmark<BS_OSU, osu_bw>;
DECLARE_INHERITED(GLUE_TYPENAME(OSUBenchmark<BS_OSU, osu_bw>), osu_bw)

