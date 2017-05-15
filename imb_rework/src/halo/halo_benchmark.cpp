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


using namespace std;

namespace ndim_halo_benchmark {

    #include "MT_benchmark.h" 
    
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

    int simple_halo(int repeat, int skip, void *in, void *out, int count, MPI_Datatype type,
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

    template class BenchmarkMT<HALOBenchmarkSuite, simple_halo>;
    DECLARE_INHERITED_TEMPLATE(GLUE_TYPENAME2(BenchmarkMT<HALOBenchmarkSuite, simple_halo>), simple_halo)
    template <> void BenchmarkMT<HALOBenchmarkSuite, simple_halo>::init_flags()
    {
        flags.insert(PT2PT);
        flags.insert(OUT_BYTES);
    }

}
