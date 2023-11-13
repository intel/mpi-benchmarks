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

 File: IMB_parse_name_ext.c 

 Implemented functions: 

 IMB_get_def_cases;
 IMB_set_bmark;

 ***************************************************************************/





#include <string.h>

#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_bnames_ext.h"
#include "IMB_comments.h"

#include "IMB_prototypes.h"


int IMB_get_def_cases(char*** defc, char*** Gcmt) {
/*

                          Initializes default benchmark names (defc) and accompanying
                          comments (Gcmt)

In/out variables:

-defc                     (type char***)
                          List of benchkark names (strings)

-Gcmt                     (type char***)
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
    BTYPES type;
#if 0
    int index;

    IMB_get_def_index(&index, Bmark->name);

    if (index < 0) {
        Bmark->RUN_MODES[0].type = BTYPE_INVALID;
        Bmark->RUN_MODES[1].type = BTYPE_INVALID;
        return;
    }
#endif

    Bmark->RUN_MODES[0].AGGREGATE = 1;
    Bmark->RUN_MODES[1].AGGREGATE = 0;

    Bmark->RUN_MODES[0].NONBLOCKING = 0;
    Bmark->RUN_MODES[1].NONBLOCKING = 0;

    Bmark->N_Modes = 2;

    Bmark->reduction = 0;

    Bmark->Ntimes = 1;

    if (!strcmp(Bmark->name, "unidir_get")) {
        strcpy(Bmark->name, "Unidir_Get");
        Bmark->Benchmark = IMB_unidir_get;
        Bmark->bench_comments = &Unidir_Get_cmt[0];
        type = SingleTransfer;
        Bmark->access = get;
    } else if (!strcmp(Bmark->name, "unidir_put")) {
        strcpy(Bmark->name, "Unidir_Put");
        Bmark->Benchmark = IMB_unidir_put;
        Bmark->bench_comments = &Unidir_Put_cmt[0];
        type = SingleTransfer;
        Bmark->access = put;
    } else if (!strcmp(Bmark->name, "bidir_get")) {
        strcpy(Bmark->name, "Bidir_Get");
        Bmark->Benchmark = IMB_bidir_get;
        Bmark->bench_comments = &Bidir_Get_cmt[0];
        type = SingleTransfer;
        Bmark->access = get;
    } else if (!strcmp(Bmark->name, "bidir_put")) {
        strcpy(Bmark->name, "Bidir_Put");
        Bmark->Benchmark = IMB_bidir_put;
        Bmark->bench_comments = &Bidir_Put_cmt[0];
        type = SingleTransfer;
        Bmark->access = put;
    } else if (!strcmp(Bmark->name, "accumulate")) {
        strcpy(Bmark->name, "Accumulate");
        Bmark->Benchmark = IMB_accumulate;
        Bmark->bench_comments = &Accumulate_cmt[0];
        type = Collective;
        Bmark->access = put;
        Bmark->reduction = 1;
    } else if (!strcmp(Bmark->name, "window")) {
        strcpy(Bmark->name, "Window");
        Bmark->Benchmark = IMB_window;
        Bmark->bench_comments = &Window_cmt[0];
        Bmark->RUN_MODES[0].AGGREGATE = -1;
        Bmark->N_Modes = 1;
        type = Collective;
        Bmark->access = no;
        Bmark->reduction = 1;
    } else {
        type = BTYPE_INVALID;
    }
    Bmark->RUN_MODES[0].type = type;
    Bmark->RUN_MODES[1].type = type;

}
