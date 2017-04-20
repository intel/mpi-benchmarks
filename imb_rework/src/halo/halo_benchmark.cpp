#include <vector>
#include <string>
#include <map>
#include <set>
#include <iostream>

#include "halo_benchmark.h" 
#include "halo_suite.h"

#define WITH_VECTOR

#define GLUE_TYPENAME2(A,B) A,B
#define GLUE_TYPENAME3(A,B,C) A,B,C

#define WRAP(NEWNAME, OLDNAME) int NEWNAME(int repeat, int skip, void *in, void *out, int count, MPI_Datatype type, \
                                       MPI_Comm comm, int rank, int size, input_benchmark_data *idata, \
                                        output_benchmark_data *odata) { \
                                    return OLDNAME(repeat, skip, in, out, count, type, comm, rank, size, idata, odata); \
}

//#define DECLARE_INHERITED_BENCHMARKHALO2(BS, FUNC, NAME) template class BenchmarkHALO<BS, FUNC>; \
//    DECLARE_INHERITED(GLUE_TYPENAME3(BenchmarkHALO<BS, FUNC >), NAME) \
//    template <> void BenchmarkHALO<BS, FUNC >::init_flags() 

#define DECLARE_INHERITED_BENCHMARKHALO(BS, FUNC, NAME) template class BenchmarkHALO<BS, FUNC>; \
    DECLARE_INHERITED(GLUE_TYPENAME2(BenchmarkHALO<BS, FUNC>), NAME) \
    template <> void BenchmarkHALO<BS, FUNC >::init_flags() 

using namespace std;

inline bool set_stride(int rank, int size, int &stride, int &group)
{
    if (stride == 0)
        stride = size/2;
    if (stride <= 0 || stride > size/2)
        return false;
    group = rank / stride;
    if ((group / 2 == size / (2 * stride)) && (size % (2*stride) != 0))
        return false;
    return true;
}

int halo(int repeat, int skip, void *in, void *out, int count, MPI_Datatype type,
               MPI_Comm comm, int rank, int size, input_benchmark_data *idata,
               output_benchmark_data *odata) {
    int group = 0;
    int stride = idata->pt2pt.stride;
    if (!set_stride(rank, size, stride, group))
        return 0;
    for (int i = 0; i < repeat; i++)
        if (group % 2 == 0) {
            MPI_Send(in, count, type, rank + stride, 0, comm);
            MPI_Recv(out, count, type, rank + stride, 0, comm, MPI_STATUS_IGNORE);
        } else {
            MPI_Recv(out, count, type, rank - stride, 0, comm, MPI_STATUS_IGNORE);
            MPI_Send(in, count, type, rank - stride, 0 , comm);
        }
    return 1;
}

DECLARE_INHERITED_BENCHMARKHALO(HALOBenchmarkSuite, halo, HaloMT) 
{
    flags.insert(DUMMY);
}


