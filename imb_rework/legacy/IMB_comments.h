/*****************************************************************************
 *                                                                           *
 * Copyright (c) 2003-2016 Intel Corporation.                                *
 * All rights reserved.                                                      *
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

[2] Intel (R) MPI Benchmarks
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
