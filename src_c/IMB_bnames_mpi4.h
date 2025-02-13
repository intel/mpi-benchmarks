/****************************************************************************
*                                                                           *
* Copyright (C) 2024 Intel Corporation                                      *
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

 ***************************************************************************/




#include "IMB_benchmark.h"

/* NAMES OF BENCHMARKS (DEFAULT CASE)*/
char *DEFC[] = {
    "Bcast_persist",
    "Allgather_persist",
    "Allgatherv_persist",
    "Gather_persist",
    "Gatherv_persist",
    "Scatter_persist",
    "Scatterv_persist",
    "Alltoall_persist",
    "Alltoallv_persist",
    "Reduce_persist",
    "Reduce_scatter_persist",
    "Allreduce_persist",
    "Barrier_persist"
};

/* NAMES OF BENCHMARKS (ALL CASE)*/
char *ALLC[] = {
    "Bcast_persist",
    "Bcast_persist_pure",
    "Allgather_persist",
    "Allgather_persist_pure",
    "Allgatherv_persist",
    "Allgatherv_persist_pure",
    "Gather_persist",
    "Gather_persist_pure",
    "Gatherv_persist",
    "Gatherv_persist_pure",
    "Scatter_persist",
    "Scatter_persist_pure",
    "Scatterv_persist",
    "Scatterv_persist_pure",
    "Alltoall_persist",
    "Alltoall_persist_pure",
    "Alltoallv_persist",
    "Alltoallv_persist_pure",
    "Reduce_persist",
    "Reduce_persist_pure",
    "Reduce_scatter_persist",
    "Reduce_scatter_persist_pure",
    "Allreduce_persist",
    "Allreduce_persist_pure",
    "Barrier_persist",
    "Barrier_persist_pure"
};
