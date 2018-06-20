/*****************************************************************************
 *                                                                           *
 * Copyright 2003-2018 Intel Corporation.                                    *
 *                                                                           *
 *****************************************************************************

This code is covered by the Community Source License (CPL), version
1.0 as published by IBM and reproduced in the file "license.txt" in the
"license" subdirectory. Redistribution in source and binary form, with
or without modification, is permitted ONLY within the regulations
contained in above mentioned license.

Use of the name and trademark "Intel(R) MPI Benchmarks" is allowed ONLY
within the regulations of the "License for Use of "Intel(R) MPI
Benchmarks" Name and Trademark" as reproduced in the file
"use-of-trademark-license.txt" in the "license" subdirectory. 

THE PROGRAM IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT
LIMITATION, ANY WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT,
MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Each Recipient is
solely responsible for determining the appropriateness of using and
distributing the Program and assumes all risks associated with its
exercise of rights under this Agreement, including but not limited to
the risks and costs of program errors, compliance with applicable
laws, damage to or loss of data, programs or equipment, and
unavailability or interruption of operations.

EXCEPT AS EXPRESSLY SET FORTH IN THIS AGREEMENT, NEITHER RECIPIENT NOR
ANY CONTRIBUTORS SHALL HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING
WITHOUT LIMITATION LOST PROFITS), HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OR
DISTRIBUTION OF THE PROGRAM OR THE EXERCISE OF ANY RIGHTS GRANTED
HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 

EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF
YOUR JURISDICTION. It is licensee's responsibility to comply with any
export regulations applicable in licensee's jurisdiction. Under
CURRENT U.S. export regulations this software is eligible for export
from the U.S. and can be downloaded by or otherwise exported or
reexported worldwide EXCEPT to U.S. embargoed destinations which
include Cuba, Iraq, Libya, North Korea, Iran, Syria, Sudan,
Afghanistan and any other country to which the U.S. has embargoed
goods and services.

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

