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
    
 File: IMB_parse_name_nbc.c 

 Implemented functions: 

 IMB_get_def_cases;
 IMB_set_bmark;

 ***************************************************************************/





#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_bnames_nbc.h"
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
    Bmark->RUN_MODES[0].NONBLOCKING = strstr(Bmark->name, "_pure") ? 0 : 1;
    Bmark->RUN_MODES[0].type = Collective;

    Bmark->reduction = 0;
    Bmark->Ntimes = Bmark->RUN_MODES[0].NONBLOCKING ? 3 : 1;
    Bmark->bench_comments = NIL_COMMENT;

    if (!strcmp(Bmark->name, "ibcast")) {
        Bmark->Benchmark = IMB_ibcast;
    } else if (!strcmp(Bmark->name, "ibcast_pure")) {
        Bmark->Benchmark = IMB_ibcast_pure;
    } else if (!strcmp(Bmark->name, "iallgather")) {
        Bmark->Benchmark = IMB_iallgather;
    } else if (!strcmp(Bmark->name, "iallgather_pure")) {
        Bmark->Benchmark = IMB_iallgather_pure;
    } else if (!strcmp(Bmark->name, "iallgatherv")) {
        Bmark->Benchmark = IMB_iallgatherv;
    } else if (!strcmp(Bmark->name, "iallgatherv_pure")) {
        Bmark->Benchmark = IMB_iallgatherv_pure;
    } else if (!strcmp(Bmark->name, "igather")) {
        Bmark->Benchmark = IMB_igather;
    } else if (!strcmp(Bmark->name, "igather_pure")) {
        Bmark->Benchmark = IMB_igather_pure;
    } else if (!strcmp(Bmark->name, "igatherv")) {
        Bmark->Benchmark = IMB_igatherv;
    } else if (!strcmp(Bmark->name, "igatherv_pure")) {
        Bmark->Benchmark = IMB_igatherv_pure;
    } else if (!strcmp(Bmark->name, "iscatter")) {
        Bmark->Benchmark = IMB_iscatter;
    } else if (!strcmp(Bmark->name, "iscatter_pure")) {
        Bmark->Benchmark = IMB_iscatter_pure;
    } else if (!strcmp(Bmark->name, "iscatterv")) {
        Bmark->Benchmark = IMB_iscatterv;
    } else if (!strcmp(Bmark->name, "iscatterv_pure")) {
        Bmark->Benchmark = IMB_iscatterv_pure;
    } else if (!strcmp(Bmark->name, "ialltoall")) {
        Bmark->Benchmark = IMB_ialltoall;
    } else if (!strcmp(Bmark->name, "ialltoall_pure")) {
        Bmark->Benchmark = IMB_ialltoall_pure;
    } else if (!strcmp(Bmark->name, "ialltoallv")) {
        Bmark->Benchmark = IMB_ialltoallv;
    } else if (!strcmp(Bmark->name, "ialltoallv_pure")) {
        Bmark->Benchmark = IMB_ialltoallv_pure;
    } else if (!strcmp(Bmark->name, "ireduce")) {
        Bmark->reduction = 1;
        Bmark->Benchmark = IMB_ireduce;
    } else if (!strcmp(Bmark->name, "ireduce_pure")) {
        Bmark->reduction = 1;
        Bmark->Benchmark = IMB_ireduce_pure;
    } else if (!strcmp(Bmark->name, "ireduce_scatter")) {
        Bmark->reduction = 1;
        Bmark->Benchmark = IMB_ireduce_scatter;
    } else if (!strcmp(Bmark->name, "ireduce_scatter_pure")) {
        Bmark->reduction = 1;
        Bmark->Benchmark = IMB_ireduce_scatter_pure;
    } else if (!strcmp(Bmark->name, "iallreduce")) {
        Bmark->reduction = 1;
        Bmark->Benchmark = IMB_iallreduce;
    } else if (!strcmp(Bmark->name, "iallreduce_pure")) {
        Bmark->reduction = 1;
        Bmark->Benchmark = IMB_iallreduce_pure;
    } else if (!strcmp(Bmark->name, "ibarrier")) {
        Bmark->Benchmark = IMB_ibarrier;
        Bmark->RUN_MODES[0].type = Sync;
    } else if (!strcmp(Bmark->name, "ibarrier_pure")) {
        Bmark->Benchmark = IMB_ibarrier_pure;
        Bmark->RUN_MODES[0].type = Sync;
    } else {
        Bmark->RUN_MODES[0].type = BTYPE_INVALID;
        return;
    }

    Bmark->name[0] = toupper(Bmark->name[0]);
}


