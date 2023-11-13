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
    
 File: IMB_parse_name_mpi1.c 

 Implemented functions: 

 IMB_get_def_cases;
 IMB_set_bmark;

 ***************************************************************************/





#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_bnames_mpi1.h"
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
    BTYPES 	type;

#if 0
    int 	index;

    IMB_get_def_index(&index, Bmark->name);

    if (index < 0) {
        Bmark->RUN_MODES[0].type = BTYPE_INVALID;
        Bmark->RUN_MODES[1].type = BTYPE_INVALID;
        return;
    }
#endif /* 0 */

    Bmark->N_Modes = 1;
    Bmark->RUN_MODES[0].AGGREGATE = -1;
    Bmark->RUN_MODES[0].NONBLOCKING = 0;

    Bmark->reduction = 0;
    Bmark->Ntimes = 1;
    Bmark->select_source = 0;

    if (!strcmp(Bmark->name, "pingpong")) {
        strcpy(Bmark->name, "PingPong");
        Bmark->Benchmark = IMB_pingpong;
        Bmark->select_source = 1;
        Bmark->bench_comments = &PingPong_cmt[0];
        type = SingleTransfer;
        Bmark->scale_time = 0.5;
    } else if (!strcmp(Bmark->name, "pingping")) {
        strcpy(Bmark->name, "PingPing");
        Bmark->Benchmark = IMB_pingping;
        Bmark->select_source = 1;
        Bmark->bench_comments = &PingPing_cmt[0];
        type = SingleTransfer;
    } else if (!strcmp(Bmark->name, "pingponganysource")) {
        strcpy(Bmark->name, "PingPongAnySource");
        Bmark->Benchmark = IMB_pingpong;
        Bmark->bench_comments = &PingPong_cmt[0];
        type = SingleTransfer;
        Bmark->scale_time = 0.5;
    } else if (!strcmp(Bmark->name, "pingpongspecificsource")) {
        strcpy(Bmark->name, "PingPongSpecificSource");
        Bmark->Benchmark = IMB_pingpong;
        Bmark->select_source = 1;
        Bmark->bench_comments = &PingPong_cmt[0];
        type = SingleTransfer;
        Bmark->scale_time = 0.5;
    } else if (!strcmp(Bmark->name, "pingpinganysource")) {
        strcpy(Bmark->name, "PingPingAnySource");
        Bmark->Benchmark = IMB_pingping;
        Bmark->bench_comments = &PingPing_cmt[0];
        type = SingleTransfer;
    } else if (!strcmp(Bmark->name, "pingpingspecificsource")) {
        strcpy(Bmark->name, "PingPingSpecificSource");
        Bmark->Benchmark = IMB_pingping;
        Bmark->select_source = 1;
        Bmark->bench_comments = &PingPing_cmt[0];
        type = SingleTransfer;
    } else if (!strcmp(Bmark->name, "sendrecv")) {
        strcpy(Bmark->name, "Sendrecv");
        Bmark->Benchmark = IMB_sendrecv;
        Bmark->bench_comments = &Sendrecv_cmt[0];
        type = ParallelTransfer;
        Bmark->scale_time = 1.0;
        Bmark->scale_bw = 2.0;
    } else if (!strcmp(Bmark->name, "exchange")) {
        strcpy(Bmark->name, "Exchange");
        Bmark->Benchmark = IMB_exchange;
        Bmark->bench_comments = &Exchange_cmt[0];
        type = ParallelTransfer;
        Bmark->scale_time = 1.0;
        Bmark->scale_bw = 4.0;
    } else if (!strcmp(Bmark->name, "allreduce")) {
        strcpy(Bmark->name, "Allreduce");
        Bmark->Benchmark = IMB_allreduce;
        type = Collective;
        Bmark->bench_comments = &Allreduce_cmt[0];
        Bmark->reduction = 1;
    } else if (!strcmp(Bmark->name, "reduce")) {
        strcpy(Bmark->name, "Reduce");
        Bmark->Benchmark = IMB_reduce;
        type = Collective;
        Bmark->bench_comments = &Reduce_cmt[0];
        Bmark->reduction = 1;
    } else if (!strcmp(Bmark->name, "reduce_scatter")) {
        strcpy(Bmark->name, "Reduce_scatter");
        Bmark->Benchmark = IMB_reduce_scatter;
        type = Collective;
        Bmark->bench_comments = &Reduce_scatter_cmt[0];
        Bmark->reduction = 1;
    } else if (!strcmp(Bmark->name, "bcast")) {
        strcpy(Bmark->name, "Bcast");
        Bmark->Benchmark = IMB_bcast;
        type = Collective;
        Bmark->bench_comments = &Bcast_cmt[0];
    } else if (!strcmp(Bmark->name, "barrier")) {
        strcpy(Bmark->name, "Barrier");
        Bmark->Benchmark = IMB_barrier;
        type = Sync;
        Bmark->bench_comments = &Barrier_cmt[0];
    } else if (!strcmp(Bmark->name, "allgather")) {
        strcpy(Bmark->name, "Allgather");
        Bmark->Benchmark = IMB_allgather;
        type = Collective;
        Bmark->bench_comments = &Allgather_cmt[0];
    } else if (!strcmp(Bmark->name, "allgatherv")) {
        strcpy(Bmark->name, "Allgatherv");
        Bmark->Benchmark = IMB_allgatherv;
        type = Collective;
        Bmark->bench_comments = &Allgatherv_cmt[0];
    } else if (!strcmp(Bmark->name, "gather")) {
        strcpy(Bmark->name, "Gather");
        Bmark->Benchmark = IMB_gather;
        type = Collective;
        Bmark->bench_comments = &Gather_cmt[0];
    } else if (!strcmp(Bmark->name, "gatherv")) {
        strcpy(Bmark->name, "Gatherv");
        Bmark->Benchmark = IMB_gatherv;
        type = Collective;
        Bmark->bench_comments = &Gatherv_cmt[0];
    } else if (!strcmp(Bmark->name, "scatter")) {
        strcpy(Bmark->name, "Scatter");
        Bmark->Benchmark = IMB_scatter;
        type = Collective;
        Bmark->bench_comments = &Scatter_cmt[0];
    } else if (!strcmp(Bmark->name, "scatterv")) {
        strcpy(Bmark->name, "Scatterv");
        Bmark->Benchmark = IMB_scatterv;
        type = Collective;
        Bmark->bench_comments = &Scatterv_cmt[0];
    } else if (!strcmp(Bmark->name, "alltoall")) {
        strcpy(Bmark->name, "Alltoall");
        Bmark->Benchmark = IMB_alltoall;
        type = Collective;
        Bmark->bench_comments = &Alltoall_cmt[0];
    } else if (!strcmp(Bmark->name, "alltoallv")) {
        strcpy(Bmark->name, "Alltoallv");
        Bmark->Benchmark = IMB_alltoallv;
        type = Collective;
        Bmark->bench_comments = &Alltoallv_cmt[0];
    } else if (!strcmp(Bmark->name, "uniband")) {
        strcpy(Bmark->name, "Uniband");
        Bmark->Benchmark = IMB_uni_bandwidth;
        Bmark->bench_comments = &Uni_bandwidth_cmt[0];
        type = ParallelTransferMsgRate;
        Bmark->scale_time = 1.0;
        Bmark->scale_bw = 1.0;
    } else if (!strcmp(Bmark->name, "biband")) {
        strcpy(Bmark->name, "Biband");
        Bmark->Benchmark = IMB_bi_bandwidth;
        Bmark->bench_comments = &Bi_bandwidth_cmt[0];
        type = ParallelTransferMsgRate;
        Bmark->scale_time = 1.0;
        Bmark->scale_bw = 2.0;
    } else {
        type = BTYPE_INVALID;
    }

    Bmark->RUN_MODES[0].type = type;
}

