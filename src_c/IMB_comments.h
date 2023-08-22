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


#ifndef IMB_COMMENTS_H
#define IMB_COMMENTS_H

#include <stdio.h>


/* DEFAULT EMPTY COMMENT */
extern char* NIL_COMMENT[];

char** Gral_cmt           = NIL_COMMENT;

char** PingPong_cmt       = NIL_COMMENT;
char** PingPing_cmt       = NIL_COMMENT;
char** Sendrecv_cmt       = NIL_COMMENT;
char** Exchange_cmt       = NIL_COMMENT;
char** Allreduce_cmt      = NIL_COMMENT;
char** Reduce_cmt         = NIL_COMMENT;
char** Reduce_scatter_cmt = NIL_COMMENT;
char** Bcast_cmt          = NIL_COMMENT;
char** Barrier_cmt        = NIL_COMMENT;
char** Allgather_cmt      = NIL_COMMENT;
char** Allgatherv_cmt     = NIL_COMMENT;
char** Gatherv_cmt        = NIL_COMMENT;
char** Gather_cmt         = NIL_COMMENT;
char** Scatterv_cmt       = NIL_COMMENT;
char** Scatter_cmt        = NIL_COMMENT;
char** Alltoall_cmt       = NIL_COMMENT;
char** Alltoallv_cmt      = NIL_COMMENT;

char** Unidir_Get_cmt     = NIL_COMMENT;
char** Unidir_Put_cmt     = NIL_COMMENT;
char** Bidir_Get_cmt      = NIL_COMMENT;
char** Bidir_Put_cmt      = NIL_COMMENT;
char** Accumulate_cmt     = NIL_COMMENT;
char** Window_cmt         = NIL_COMMENT;


char** Write_Indv_cmt     = NIL_COMMENT;
char** Write_Shared_cmt   = NIL_COMMENT;
char** Write_Priv_cmt     = NIL_COMMENT;
char** Write_Expl_cmt     = NIL_COMMENT;
char** Read_Indv_cmt      = NIL_COMMENT;
char** Read_Shared_cmt    = NIL_COMMENT;
char** Read_Priv_cmt      = NIL_COMMENT;
char** Read_Expl_cmt      = NIL_COMMENT;
char** Open_cmt           = NIL_COMMENT;
char** Bi_bandwidth_cmt   = NIL_COMMENT;
char** Uni_bandwidth_cmt  = NIL_COMMENT;

char *Compare_and_swap_cmt[] = {"Uses MPI_INT data type", NULL};

char *Truly_passive_put_cmt[] = 
             {"The benchmark measures execution time of MPI_Put for 2 cases:", 
              "1) The target is waiting in MPI_Barrier call (t_pure value)",
              "2) The target performs computation and then enters MPI_Barrier routine (t_ovrl value)",
               NULL};



#endif // IMB_COMMENTS_H
