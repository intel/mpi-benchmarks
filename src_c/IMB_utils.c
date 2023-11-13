/****************************************************************************
*                                                                           *
* Copyright (C) 2023 Intel Corporation                                      *
*                                                                           *
*****************************************************************************

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ***************************************************************************

For more documentation than found here, see

[1] doc/ReadMe_IMB.txt

[2] Intel(R) MPI Benchmarks
    Users Guide and Methodology Description
    In
    doc/IMB_Users_Guide.pdf

 File: IMB_utils.c

 Implemented functions:

    IMB_Barrier;

 ***************************************************************************/

#include "mpi.h"

#define IMB_BARRIER_TAG 1000
int IMB_internal_barrier = 0;
/*

                      MPI-independent implementation of barrier syncronization.
                      Implements  Dissemination barrier algorithm.

Input variables:

-comm                 (type MPI_Comm)
                      Communicator for barrier syncronization

*/

void IMB_Barrier(MPI_Comm comm) {
    int size = 0;
    int rank = 0;

    int mask = 0x1;
    int dst, src;

    int tmp = 0;

    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    for (; mask < size; mask <<= 1) {
        dst = (rank + mask) % size;
        src = (rank - mask + size) % size;
        MPI_Sendrecv(&tmp, 0, MPI_BYTE, dst, IMB_BARRIER_TAG,
                     &tmp, 0, MPI_BYTE, src, IMB_BARRIER_TAG,
                     comm, MPI_STATUS_IGNORE);
    }
}

