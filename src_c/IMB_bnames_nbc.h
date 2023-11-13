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
    
 ***************************************************************************/




#include "IMB_benchmark.h"

/* NAMES OF BENCHMARKS (DEFAULT CASE)*/
char *DEFC[] = {
    "Ibcast",
    "Iallgather",
    "Iallgatherv",
    "Igather",
    "Igatherv",
    "Iscatter",
    "Iscatterv",
    "Ialltoall",
    "Ialltoallv",
    "Ireduce",
    "Ireduce_scatter",
    "Iallreduce",
    "Ibarrier"
};

/* NAMES OF BENCHMARKS (ALL CASE)*/
char *ALLC[] = {
    "Ibcast",
    "Ibcast_pure",
    "Iallgather",
    "Iallgather_pure",
    "Iallgatherv",
    "Iallgatherv_pure",
    "Igather",
    "Igather_pure",
    "Igatherv",
    "Igatherv_pure",
    "Iscatter",
    "Iscatter_pure",
    "Iscatterv",
    "Iscatterv_pure",
    "Ialltoall",
    "Ialltoall_pure",
    "Ialltoallv",
    "Ialltoallv_pure",
    "Ireduce",
    "Ireduce_pure",
    "Ireduce_scatter",
    "Ireduce_scatter_pure",
    "Iallreduce",
    "Iallreduce_pure",
    "Ibarrier",
    "Ibarrier_pure"
};

