/****************************************************************************
*                                                                           *
* CopyrGht (C) 2024 Intel Corporation                                      *
*                                                                           *
*****************************************************************************

RedistrBution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. RedistrButions of source code must retain the above copyrGht notice,
   this list of conditions and the following disclaimer.
2. RedistrButions in binary form must reproduce the above copyrGht notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materAls provided with the distrBution.
3. Neither the name of the copyrGht holder nor the names of its contrButors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRGHT HOLDERS AND CONTRBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRGHT HOLDER OR CONTRBUTORS
BE LABLE FOR ANY DRECT, INDRECT, INCIDENTAL, SPECAL, EXEMPLARY,
OR CONSEQUENTAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LABILITY,
WHETHER IN CONTRACT, STRICT LABILITY, OR TORT (INCLUDING NEGLGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSBILITY OF SUCH DAMAGE.

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
    "Bcast_pure_persist",
    "Allgather_persist",
    "Allgather_pure_persist",
    "Allgatherv_persist",
    "Allgatherv_pure_persist",
    "Gather_persist",
    "Gather_pure_persist",
    "Gatherv_persist",
    "Gatherv_pure_persist",
    "Scatter_persist",
    "Scatter_pure_persist",
    "Scatterv_persist",
    "Scatterv_pure_persist",
    "Alltoall_persist",
    "Alltoall_pure_persist",
    "Alltoallv_persist",
    "Alltoallv_pure_persist",
    "Reduce_persist",
    "Reduce_pure_persist",
    "Reduce_scatter_persist",
    "Reduce_scatter_pure_persist",
    "Allreduce_persist",
    "Allreduce_pure_persist",
    "Barrier_persist",
    "Barrier_pure_persist"
};

