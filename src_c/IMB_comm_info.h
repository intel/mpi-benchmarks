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





#ifndef _COMM_INFO_H
#define _COMM_INFO_H

#include "IMB_declare.h"

typedef enum {
    CT_BASE        = 0,
    CT_BASE_VEC    = 1,
    CT_RESIZE      = 2,
    CT_RESIZE_VEC  = 3
} CONTIG_TYPES;

#if (defined EXT || defined RMA || defined MPIIO)
    typedef enum {
        AM_ERROR       = -1,
        AM_TURN_OFF    =  0,
        AM_TURN_ON     =  1,
        AM_TURN_MULTI  =  2
    } AGGREGATE_MODE;
#endif

#ifdef MPIIO
typedef struct { int Locsize; MPI_Offset Offset; int Totalsize;} SPLITTING;
#endif

#ifdef GPU_ENABLE
typedef enum {
    MAT_CPU    = 0,
    MAT_DEVICE = 1,
    MAT_HOST   = 2,
    MAT_SHARED = 3
} MEM_ALLOC_TYPE;
#endif //GPU_ENABLE

struct comm_info {
/* Communication information as for MPI-1/2 parts */

    int         w_num_procs;        /* number of procs in COMM_WORLD            */
    int         w_rank;             /* rank of actual process in COMM_WORLD     */

    int         NP;                 /* #processes participating in benchmarks   */
    int         px, py;             /* processes are part of px x py topology   */

    MPI_Comm    communicator;       /* underlying communicator for benchmark(s) */

    int         num_procs;          /* number of processes in communicator      */
    int         rank;               /* rank of actual process in communicator   */
    int         root_shift;         /* switch for root change at each iteration */
    int         sync;               /* switch for rank synchronization after each iter */
    int         size_scale;

    CONTIG_TYPES    contig_type;
    MPI_Datatype    s_data_type;    /* data type of sent data                   */
    MPI_Datatype    r_data_type;    /* data type of received data               */

    MPI_Datatype    red_data_type;  /* data type of reduced data               */
    MPI_Op      op_type;            /* operation type                          */

    int         zero_size;
    int         pair0, pair1;       /* process pair                            */
    int         select_tag;         /* 0/1 for tag selection off/on            */
    int         select_source;      /* 0/1 for sender selection off/on         */

    void*       s_buffer;           /* send    buffer                          */
    assign_type*    s_data;         /* assign_type equivalent of s_buffer      */
    size_t      s_alloc;            /* #bytes allocated in s_buffer            */
    void*       r_buffer;           /* receive buffer                          */
    assign_type*    r_data;         /* assign_type equivalent of r_buffer      */
    size_t          r_alloc;        /* #bytes allocated in r_buffer            */

    /* IMB 3.1 << */
    float       max_mem, used_mem;  /* max. allowed / used GBytes for all      */
    /* message  buffers                        */
    /* >> IMB 3.1  */

    int     n_lens;         /* # of selected lengths by -msglen option */
    int*    msglen;         /* list of  "       "                  "   */

    int     group_mode;     /* Mode of running groups (<0,0,>0)        */
    int     n_groups;       /* No. of independent groups               */
    int     group_no;       /* own group index                         */
    int*    g_sizes;        /* array of group sizes                    */
    int*    g_ranks;        /* w_ranks constituting the groups         */

    int*    sndcnt;         /* send count argument for global ops.     */
    int*    sdispl;         /* displacement argument for global ops.   */
    int*    reccnt;         /* recv count argument for global ops.     */
    int*    rdispl;         /* displacement argument for global ops.   */

    /* IMB 3.2.3 << */
    int     min_msg_log;
    int     max_msg_log;
    /* >> IMB 3.2.3  */

    MPI_Errhandler  ERR;

    int warm_up;

    int msg_pause;

    int max_win_size;

#ifdef MPIIO
    /*   FILE INFORMATION     */
    char*       filename;
    MPI_Comm    File_comm;
    int         File_num_procs;
    int         all_io_procs;
    int         File_rank;

    MPI_File    fh;

    MPI_Datatype    etype;
    Type_Size       e_size;
    MPI_Datatype    filetype;

    SPLITTING   split;
    int         amode;
    MPI_Info    info;

    /* View: */
    MPI_Offset  disp;
    char*       datarep;
    MPI_Datatype    view;
    MPI_Errhandler  ERRF;
#endif

#if (defined EXT || defined RMA)
    MPI_Win         WIN;
    MPI_Info        info;
    MPI_Errhandler  ERRW;
#endif

#if (defined EXT || defined RMA || defined MPIIO)
    AGGREGATE_MODE aggregate_mode;  /*turn on different types of aggregate modes*/
#endif

#ifdef GPU_ENABLE
    MEM_ALLOC_TYPE mem_alloc_type;
#endif //GPU_ENABLE

};

#endif
