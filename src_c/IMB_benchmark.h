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




#ifndef __Bmark_h__
#define __Bmark_h__

#include "IMB_mem_info.h"

/* Classification of benchmarks */

typedef enum {
    BTYPE_INVALID=-1,
    SingleTransfer,
    ParallelTransfer,
    ParallelTransferMsgRate,
    Collective,
    Sync,
    MultPassiveTransfer,
    SingleElementTransfer
} BTYPES;


typedef struct cmode {
    int AGGREGATE ;   /* -1/0/1 for default/NON AGGREGATE/AGGREGATE */
    int NONBLOCKING  ;   /* 0/1 for no/yes */
    int BIDIR;           /* 0/1 for no/yes */
    BTYPES type;
} *MODES;

typedef enum {
    imode_off         = 0,
    imode_dynamic     = 1,
    imode_multiple_np = 2,
    imode_auto        = 3,
    imode_invalid          /* always last */
} IMODE;

/* IMB 3.1 << */
struct iter_schedule {
    int     msgspersample, msgs_nonaggr, overall_vol;
    /* evtl override for default parameters MSGSPERSAMPLE, MSGS_NONAGGR, OVERALL_VOL */

    int     n_sample, n_sample_prev;

    /* dynamic adaptation eventually */
    IMODE   iter_policy;   /* enum for request */
    int*    numiters;      /* #iterations in case of -msglen request */
    float   secs;          /* max. seconds to run if dynamic requested */

    /* if off-cache is requested: */
    int     off_cache;  /* global bool for request */
    int     use_off_cache;  /* per benchmark bool for request */
    float   cache_size;
    int     cache_line_size;
    int     s_cache_iter;  /* no. of send buffers needed to sufficiently exceed cache size */
    size_t  s_offs;        /* offset between send buffers to not hit same cache line */
    int     r_cache_iter;  /*            dto for recv - buffers              */
    size_t  r_offs;        /*            dto for recv - buffers              */
};

/* >> IMB 3.1  */

#define X_MODES 2

typedef enum { put, get, no, local } DIRECTION;

#ifdef MPIIO

typedef enum {
    nothing = -1,
    priv,
    explic,
    indv_block,
    shared
} POSITIONING;
#else

typedef int POSITIONING;
#define nothing -1
#endif

/* Descriptor for benchmarks */
/* IMB 3.1 << */
// failure flags for single samples
#define SAMPLE_FAILED_MEMORY        (-111111)
#define SAMPLE_FAILED_INT_OVERFLOW  (-111112)
#define SAMPLE_FAILED_TIME_OUT      (-111113)
/* >> IMB 3.1  */

struct Bench {
    char*  name;
    char** bench_comments;                                  /* Comments from header => Comments.h */

    DIRECTION access;

    int reduction;                                          /* reduction-type y/n */

    int N_Modes;
    struct cmode RUN_MODES[X_MODES];

    void (*Benchmark)(struct comm_info* c_info,int size,    /* Pointer to function runnning the benchmark */
/* IMB 3.1 << */
                      struct iter_schedule* ITERATIONS,MODES RUN_MODE,double* time);
/* >> IMB 3.1  */

    double  scale_time, scale_bw;                           /* Scaling of timings and bandwidth */
    int     Ntimes;
/* IMB 3.1 << */
    int     sample_failure;
// only for -DCHECK purposes:
    int     success;
/* >> IMB 3.1  */

#ifdef MPI1
    int     select_source;                                  /* to distinguish PingPong/PingPing with and without MPI_ANY_SOURCE*/
#endif

#ifdef MPIIO
  POSITIONING fpointer;
#endif
};
/* IMB_3.0 */

#define LIST_END        -2
#define LIST_INVALID    -1

#endif
