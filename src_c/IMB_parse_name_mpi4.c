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
    
 File: IMB_parse_name_nbc.c 

 Implemented functions: 

 IMB_get_def_cases;
 IMB_set_bmark;

 ***************************************************************************/





#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_bnames_mpi4.h"
#include "IMB_comments.h"

#include "IMB_prototypes.h"
#include <ctype.h>

int IMB_get_def_cases(char*** defc, char*** Gcmt) {
/*

                      Initializes default benchmark names (defc) and accompanying
                      comments (Gcmt)

In/out variables:

-defc                 (type char***)
                      List of benchkark names (strings)

-Gcmt                 (type char***)
                      List of general comments (strings)

*/
    *defc = &DEFC[0];
    *Gcmt = &Gral_cmt[0];
    return (int)(sizeof(DEFC) / sizeof(char*));
}

int IMB_get_all_cases(char*** allc) {
/*

                      Initializes default benchmark names (defc) and accompanying
                      comments (Gcmt)

In/out variables:

-defc                 (type char***)
                      List of benchkark names (strings)

-Gcmt                 (type char***)
                      List of general comments (strings)

*/
    *allc = &ALLC[0];
    return (int)(sizeof(ALLC) / sizeof(char*));
}


void IMB_set_bmark(struct Bench* Bmark) {
/*

In/out variables:

-Bmark                (type struct Bench*)
                      (For explanation of struct Bench type:
                      describes all aspects of modes of a benchmark;
                      see [1] for more information)

                      On input, only the name of the benchmark is set.
                      On output, all necessary run modes are set accordingly

*/
    Bmark->N_Modes = 1;
    Bmark->RUN_MODES[0].AGGREGATE = -1;
    Bmark->RUN_MODES[0].NONBLOCKING = strstr(Bmark->name, "_persist_pure") ? 0 : 1;
    Bmark->RUN_MODES[0].type = Collective;

    Bmark->reduction = 0;
    Bmark->Ntimes = Bmark->RUN_MODES[0].NONBLOCKING ? 3 : 1;
    Bmark->bench_comments = NIL_COMMENT;

    if (!strcmp(Bmark->name, "bcast_persist")) {
        Bmark->Benchmark = IMB_bcast_persist;
    } else if (!strcmp(Bmark->name, "bcast_persist_pure")) {
        Bmark->Benchmark = IMB_bcast_persist_pure;
    } else if (!strcmp(Bmark->name, "allgather_persist")) {
        Bmark->Benchmark = IMB_allgather_persist;
    } else if (!strcmp(Bmark->name, "allgather_persist_pure")) {
        Bmark->Benchmark = IMB_allgather_persist_pure;
    } else if (!strcmp(Bmark->name, "allgatherv_persist")) {
        Bmark->Benchmark = IMB_allgatherv_persist;
    } else if (!strcmp(Bmark->name, "allgatherv_persist_pure")) {
        Bmark->Benchmark = IMB_allgatherv_persist_pure;
    } else if (!strcmp(Bmark->name, "gather_persist")) {
        Bmark->Benchmark = IMB_gather_persist;
    } else if (!strcmp(Bmark->name, "gather_persist_pure")) {
        Bmark->Benchmark = IMB_gather_persist_pure;
    } else if (!strcmp(Bmark->name, "gatherv_persist")) {
        Bmark->Benchmark = IMB_gatherv_persist;
    } else if (!strcmp(Bmark->name, "gatherv_persist_pure")) {
        Bmark->Benchmark = IMB_gatherv_persist_pure;
    } else if (!strcmp(Bmark->name, "scatter_persist")) {
        Bmark->Benchmark = IMB_scatter_persist;
    } else if (!strcmp(Bmark->name, "scatter_persist_pure")) {
        Bmark->Benchmark = IMB_scatter_persist_pure;
    } else if (!strcmp(Bmark->name, "scatterv_persist")) {
        Bmark->Benchmark = IMB_scatterv_persist;
    } else if (!strcmp(Bmark->name, "scatterv_persist_pure")) {
        Bmark->Benchmark = IMB_scatterv_persist_pure;
    } else if (!strcmp(Bmark->name, "alltoall_persist")) {
        Bmark->Benchmark = IMB_alltoall_persist;
    } else if (!strcmp(Bmark->name, "alltoall_persist_pure")) {
        Bmark->Benchmark = IMB_alltoall_persist_pure;
    } else if (!strcmp(Bmark->name, "alltoallv_persist")) {
        Bmark->Benchmark = IMB_alltoallv_persist;
    } else if (!strcmp(Bmark->name, "alltoallv_persist_pure")) {
        Bmark->Benchmark = IMB_alltoallv_persist_pure;
    } else if (!strcmp(Bmark->name, "reduce_persist")) {
        Bmark->reduction = 1;
        Bmark->Benchmark = IMB_reduce_persist;
    } else if (!strcmp(Bmark->name, "reduce_persist_pure")) {
        Bmark->reduction = 1;
        Bmark->Benchmark = IMB_reduce_persist_pure;
    } else if (!strcmp(Bmark->name, "reduce_scatter_persist")) {
        Bmark->reduction = 1;
        Bmark->Benchmark = IMB_reduce_scatter_persist;
    } else if (!strcmp(Bmark->name, "reduce_scatter_persist_pure")) {
        Bmark->reduction = 1;
        Bmark->Benchmark = IMB_reduce_scatter_persist_pure;
    } else if (!strcmp(Bmark->name, "allreduce_persist")) {
        Bmark->reduction = 1;
        Bmark->Benchmark = IMB_allreduce_persist;
    } else if (!strcmp(Bmark->name, "allreduce_persist_pure")) {
        Bmark->reduction = 1;
        Bmark->Benchmark = IMB_allreduce_persist_pure;
    } else if (!strcmp(Bmark->name, "barrier_persist")) {
        Bmark->Benchmark = IMB_barrier_persist;
        Bmark->RUN_MODES[0].type = Sync;
    } else if (!strcmp(Bmark->name, "barrier_persist_pure")) {
        Bmark->Benchmark = IMB_barrier_persist_pure;
        Bmark->RUN_MODES[0].type = Sync;
    } else {
        Bmark->RUN_MODES[0].type = BTYPE_INVALID;
        return;
    }

    Bmark->name[0] = toupper(Bmark->name[0]);
}
