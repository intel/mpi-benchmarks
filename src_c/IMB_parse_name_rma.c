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

#include "IMB_bnames_rma.h"
#include "IMB_comments.h"

#include "IMB_prototypes.h"
#include <ctype.h>

int IMB_get_def_cases(char*** defc, char*** Gcmt) {
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


void IMB_set_bmark(struct Bench* bmark) {
    BTYPES type;

    bmark->bench_comments = NIL_COMMENT;
    bmark->N_Modes = 2;
    bmark->Ntimes = 1;
    bmark->reduction = 0;

    bmark->RUN_MODES[0].AGGREGATE = 0;
    bmark->RUN_MODES[1].AGGREGATE = 1;
    bmark->RUN_MODES[0].NONBLOCKING = 0;
    bmark->RUN_MODES[1].NONBLOCKING = 0;
    bmark->RUN_MODES[0].BIDIR = 0;
    bmark->RUN_MODES[1].BIDIR = 0;


    if (!strcmp(bmark->name, "unidir_put")) {
        bmark->Benchmark = IMB_rma_single_put;
        type = SingleTransfer;
        bmark->access = put;
    } else if (!strcmp(bmark->name, "unidir_get")) {
        bmark->Benchmark = IMB_rma_single_get;
        type = SingleTransfer;
        bmark->access = get;
    } else if (!strcmp(bmark->name, "bidir_put")) {
        bmark->Benchmark = IMB_rma_single_put;
        type = SingleTransfer;
        bmark->access = put;
        bmark->RUN_MODES[0].BIDIR = 1;
        bmark->RUN_MODES[1].BIDIR = 1;

    } else if (!strcmp(bmark->name, "bidir_get")) {
        bmark->Benchmark = IMB_rma_single_get;
        type = SingleTransfer;
        bmark->access = get;
        bmark->RUN_MODES[0].BIDIR = 1;
        bmark->RUN_MODES[1].BIDIR = 1;

    } else if (!strcmp(bmark->name, "put_local")) {
        bmark->Benchmark = IMB_rma_put_local;
        type = SingleTransfer;
        bmark->access = put;
    } else if (!strcmp(bmark->name, "get_local")) {
        bmark->Benchmark = IMB_rma_get_local;
        type = SingleTransfer;
        bmark->access = get;
    } else if (!strcmp(bmark->name, "put_all_local")) {
        bmark->N_Modes = 1;
        bmark->RUN_MODES[0].AGGREGATE = -1;
        bmark->Benchmark = IMB_rma_put_all_local;
        type = MultPassiveTransfer;
        bmark->access = put;
    } else if (!strcmp(bmark->name, "get_all_local")) {
        bmark->N_Modes = 1;
        bmark->RUN_MODES[0].AGGREGATE = -1;
        bmark->Benchmark = IMB_rma_get_all_local;
        type = MultPassiveTransfer;
        bmark->access = get;
    } else if (!strcmp(bmark->name, "one_put_all")) {
        bmark->N_Modes = 1;
        bmark->Benchmark = IMB_rma_put_all;
        type = MultPassiveTransfer;
        bmark->access = put;
        bmark->RUN_MODES[0].AGGREGATE = -1;
    } else if (!strcmp(bmark->name, "one_get_all")) {
        bmark->N_Modes = 1;
        bmark->Benchmark = IMB_rma_get_all;
        type = MultPassiveTransfer;
        bmark->access = get;
        bmark->RUN_MODES[0].AGGREGATE = -1;
    } else if (!strcmp(bmark->name, "all_put_all")) {
        bmark->N_Modes = 1;
        bmark->Benchmark = IMB_rma_put_all;
        type = Collective;
        bmark->access = put;
        bmark->RUN_MODES[0].AGGREGATE = -1;
    } else if (!strcmp(bmark->name, "all_get_all")) {
        bmark->N_Modes = 1;
        bmark->Benchmark = IMB_rma_get_all;
        type = Collective;
        bmark->access = get;
        bmark->RUN_MODES[0].AGGREGATE = -1;
    } else if (!strcmp(bmark->name, "exchange_put")) {
        bmark->N_Modes = 1;
        bmark->Benchmark = IMB_rma_exchange_put;
        type = Collective;
        bmark->access = put;
        bmark->RUN_MODES[0].AGGREGATE = -1;
    } else if (!strcmp(bmark->name, "exchange_get")) {
        bmark->N_Modes = 1;
        bmark->Benchmark = IMB_rma_exchange_get;
        type = Collective;
        bmark->access = get;
        bmark->RUN_MODES[0].AGGREGATE = -1;
    } else if (!strcmp(bmark->name, "accumulate")) {
        bmark->Benchmark = IMB_rma_accumulate;
        type = SingleTransfer;
        bmark->access = put;
        bmark->reduction = 1;
    } else if (!strcmp(bmark->name, "get_accumulate")) {
        bmark->Benchmark = IMB_rma_get_accumulate;
        type = SingleTransfer;
        bmark->access = put;
        bmark->reduction = 1;
    } else if (!strcmp(bmark->name, "fetch_and_op")) {
        bmark->Benchmark = IMB_rma_fetch_and_op;
        type = SingleElementTransfer;
        bmark->access = put;
        bmark->reduction = 1;
    } else if (!strcmp(bmark->name, "compare_and_swap")) {
        bmark->Benchmark = IMB_rma_compare_and_swap;
        type = SingleElementTransfer;
        bmark->access = put;
        bmark->bench_comments = Compare_and_swap_cmt;
    } else if (!strcmp(bmark->name, "truly_passive_put")) {
        bmark->Benchmark = IMB_rma_passive_put;
        type = SingleTransfer;
        bmark->access = put;
        bmark->N_Modes = 1;
        bmark->Ntimes = 2;
        bmark->RUN_MODES[0].AGGREGATE = -1;
        bmark->RUN_MODES[0].NONBLOCKING = 1;
        bmark->bench_comments = Truly_passive_put_cmt;
    } else {
        bmark->RUN_MODES[0].type = BTYPE_INVALID;
        bmark->RUN_MODES[1].type = BTYPE_INVALID;
        return;
    }

    bmark->RUN_MODES[0].type = type;
    bmark->RUN_MODES[1].type = type;
    bmark->name[0] = toupper(bmark->name[0]);
}
