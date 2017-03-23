#include <vector>
#include <string>
#include <map>
#include <set>
#include <iostream>

#include "MT_benchmark.h" 
#include "MT_suite.h"

#define GLUE_TYPENAME(A,B) A,B

using namespace std;

int immb_pt2pt(int repeat, void *in, void *out, int count, MPI_Datatype type,
        MPI_Comm comm, int rank, int size, int root, int stride) {
    if (stride == 0)
        stride = size/2;
    if (stride <= 0 || stride > size/2)
        return 0;
    int group = rank / stride;
    if ((group / 2 == size / (2 * stride)) && (size % (2*stride) != 0)) 
        return 0;
    for (int i = 0; i < repeat; i++)
        if (group % 2 == 0) {
            MPI_Send(in, count, type, rank+stride, 0, comm);
            MPI_Recv(out, count, type, rank+stride, 0, comm, MPI_STATUS_IGNORE);
        } else {
            MPI_Recv(out, count, type, rank-stride, 0, comm, MPI_STATUS_IGNORE);
            MPI_Send(in, count, type, rank-stride, 0, comm);
        }
    return 1;
}


template class PingPongMT<MTBenchmarkSuite, immb_pt2pt>;
DECLARE_INHERITED(GLUE_TYPENAME(PingPongMT<MTBenchmarkSuite, immb_pt2pt>), PingPongMT)

