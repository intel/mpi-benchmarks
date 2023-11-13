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

 File: IMB_parse_name_io.c 

 Implemented functions: 

 IMB_get_def_cases;
 IMB_set_bmark;

 ***************************************************************************/





#include <string.h>

#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_bnames_io.h"
#include "IMB_comments.h"

#include "IMB_prototypes.h"

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

-allc                 (type char***)
                      List of benchkark names (strings)

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
    char *tmp_name;
    int md, nam_beg;

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


    Bmark->reduction = 0;

    Bmark->Ntimes = 1;
    md = 0;
    nam_beg = 2;

    if (Bmark->name[0] == 's') {
        type = SingleTransfer;
        Bmark->name[0] = 'S';
        if (Bmark->name[2] == 'i') {
            md = 1;
            nam_beg++;
            Bmark->name[2] = 'I';
        }
    }

    if (Bmark->name[0] == 'p') {
        type = ParallelTransfer;
        Bmark->name[0] = 'P';
        if (Bmark->name[2] == 'i') {
            md = 1;
            nam_beg++;
            Bmark->name[2] = 'I';
        }
    }

    if (Bmark->name[0] == 'c') {
        type = Collective;
        Bmark->name[0] = 'C';
        if (Bmark->name[2] == 'i') {
            md = 1;
            nam_beg++;
            Bmark->name[2] = 'I';
        }
    }

    if (md) do_nonblocking = 1;
    Bmark->Ntimes += md;

    Bmark->RUN_MODES[0].NONBLOCKING = md;
    Bmark->RUN_MODES[1].NONBLOCKING = md;

    tmp_name = Bmark->name + nam_beg;

    if (!strcmp(tmp_name, "write_indv")) {
        strcpy(tmp_name, "Write_Indv");
        Bmark->Benchmark = IMB_write_indv;
        Bmark->bench_comments = &Write_Indv_cmt[0];

        Bmark->access = put;
        Bmark->fpointer = indv_block;
    } else if (!strcmp(tmp_name, "write_shared")) {
        strcpy(tmp_name, "Write_Shared");
        Bmark->Benchmark = IMB_write_shared;
        Bmark->bench_comments = &Write_Shared_cmt[0];

        Bmark->access = put;
        Bmark->fpointer = shared;
    } else if (!strcmp(tmp_name, "write_priv")) {
        strcpy(tmp_name, "Write_Priv");
        Bmark->Benchmark = IMB_write_indv;
        Bmark->bench_comments = &Write_Priv_cmt[0];

        Bmark->access = put;
        Bmark->fpointer = priv;
    } else if (!strcmp(tmp_name, "write_expl")) {
        strcpy(tmp_name, "Write_Expl");
        Bmark->Benchmark = IMB_write_expl;
        Bmark->bench_comments = &Write_Expl_cmt[0];

        Bmark->access = put;
        Bmark->fpointer = explic;
    } else if (!strcmp(tmp_name, "read_indv")) {
        strcpy(tmp_name, "Read_Indv");
        Bmark->Benchmark = IMB_read_indv;
        Bmark->bench_comments = &Read_Indv_cmt[0];

        Bmark->access = get;
        Bmark->fpointer = indv_block;
    } else if (!strcmp(tmp_name, "read_shared")) {
        strcpy(tmp_name, "Read_Shared");
        Bmark->Benchmark = IMB_read_shared;
        Bmark->bench_comments = &Read_Shared_cmt[0];

        Bmark->access = get;
        Bmark->fpointer = shared;
    } else if (!strcmp(tmp_name, "read_priv")) {
        strcpy(tmp_name, "Read_Priv");
        Bmark->Benchmark = IMB_read_indv;
        Bmark->bench_comments = &Read_Priv_cmt[0];

        Bmark->access = get;
        Bmark->fpointer = priv;
    } else if (!strcmp(tmp_name, "read_expl")) {
        strcpy(tmp_name, "Read_Expl");
        Bmark->Benchmark = IMB_read_expl;
        Bmark->bench_comments = &Read_Expl_cmt[0];

        Bmark->access = get;
        Bmark->fpointer = explic;
    } else if (!strcmp(Bmark->name, "open_close")) {
        strcpy(Bmark->name, "Open_Close");
        Bmark->Benchmark = IMB_open_close;
        Bmark->bench_comments = &Open_cmt[0];

        type = Sync;
        Bmark->RUN_MODES[0].AGGREGATE = -1;

        Bmark->access = no;
        Bmark->fpointer = explic;
    } else {
        type = BTYPE_INVALID;
    }

    if (Bmark->access == no) Bmark->Ntimes = 1;

    Bmark->RUN_MODES[0].type = type;
    Bmark->RUN_MODES[1].type = type;

    if (type == SingleTransfer) Bmark->fpointer = priv;

    if (Bmark->access == get || Bmark->access == no ||
        md)
        Bmark->N_Modes = 1;
    else
        Bmark->N_Modes = 2;
}

