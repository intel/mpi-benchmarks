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



#ifndef _DECLARE_H
#define _DECLARE_H

#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stddef.h>
#include <assert.h>
#include "IMB_appl_errors.h"
#include "IMB_err_check.h"

#ifndef WIN_IMB
#include <strings.h>
#endif

/* THERE IS INCOSISTENT DEFINITION OF MPI_Type_Size arguments */
typedef int Type_Size;          /*correct MPI standard  */
/*typedef MPI_Aint Type_Size;*/ /*on SGI, HITACHI SR2201*/

#ifdef MPIIO
#include "IMB_settings_io.h"
#else
#include "IMB_settings.h"
#endif /*MPIIO*/

#include "IMB_comm_info.h"

extern int num_alloc, num_free;


#undef DEBUG

#ifdef DEBUG

#define DIAGNOSTICS(text,c_info,buf,loclen,totlen,j_sample,pos) {if(j_sample <=10) IMB_show((text),(c_info),(buf),(loclen),(totlen),(j_sample),(pos));}

extern FILE* dbg_file;
extern char* dbgf_name;

#define DBG_I1(text,i1) {fprintf(unit,"%s %d\n",(text),(i1));fflush(unit);}
#define DBG_I2(text,i1,i2) {fprintf(unit,"%s %d %d\n",(text),(i1),(i2));fflush(unit);}
#define DBG_I3(text,i1,i2,i3) {fprintf(unit,"%s %d %d %d\n",(text),(i1),(i2),(i3));fflush(unit);}

#define DBGF_I1(text,i1) {fprintf(dbg_file,"%s %d\n",(text),(i1));fflush(dbg_file);}
#define DBGF_I2(text,i1,i2) {fprintf(dbg_file,"%s %d %d\n",(text),(i1),(i2));fflush(dbg_file);}
#define DBGF_I3(text,i1,i2,i3) {fprintf(dbg_file,"%s %d %d\n",(text),(i1),(i2),(i3));fflush(dbg_file);}

#define DBGF_IARR(text,N,ia)\
{int i; fprintf(dbg_file,"%s ",text);for(i=0; i<N; i++) {fprintf(dbg_file,"%d ",(ia)[i]);};fprintf(dbg_file,"\n");fflush(dbg_file);}

#else /*DEBUG*/

#define DIAGNOSTICS(text,c_info,buf,loclen,totlen,j_sample,pos) 

#define DBG_I1(text,i1)
#define DBG_I2(text,i1,i2)
#define DBG_I3(text,i1,i2,i3)

#define DBGF_I1(text,i1) 
#define DBGF_I2(text,i1,i2) 
#define DBGF_I3(text,i1,i2,i3) 

#define DBGF_IARR(text,N,ia)

#endif /*DEBUG*/

#define CHK_NO_FAULT	((size_t) -1)
#define ITER_MIN(iters) min(iters->n_sample, iters->r_cache_iter) 

#ifdef CHECK
#define CHK_DIFF(text,\
             c_info, RECEIVED, buf_pos,\
             Locsize, Totalsize, unit_size,\
             mode, pos,\
             n_sample, j_sample,\
             source, diff )\
        IMB_chk_diff(\
             (text),\
             (c_info), (RECEIVED), (buf_pos), \
             (Locsize), (Totalsize), (unit_size), \
             (mode), (pos),\
             (n_sample), (j_sample),\
             (source), (diff) )\


#define CHK_STOP {if(err_flag) return;}
#define CHK_BRK  {if(err_flag) {IMB_err_hand(0,1); break;}}

/*extern long   IMB_compute_crc (char* buf, int size);*/
extern long   r_check,s_check;
extern double defect;
extern double *all_defect;


#else /*CHECK*/
#define CHK_DIFF(text,\
             c_info, RECEIVED, buf_pos,\
             Locsize, Totalsize, unit_size,\
             mode, pos,\
             n_sample, j_sample,\
             source, diff )
#define CHK_STOP
#define CHK_BRK
#endif /*CHECK*/

/*   Filename for results output; insert empty string for stdout */
#define OUTPUT_FILENAME  ""

#define SCALE 1000000

/*   WIDTH OF OUTPUT TABLE */
#define ow_format 13
#define out_fields 8

/* IMB 3.1 << */
/* include Windows case */
#ifndef WIN_IMB
#ifndef __cplusplus
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#endif
#endif /*WIN_IMB*/
/* >> IMB 3.1  */

#ifdef BUFFERS_INT
#define A_ABS(i) abs((i))
#endif

#ifdef BUFFERS_FLOAT
#define A_ABS(x) fabs((x))
#endif

typedef enum {
    MIN = 0,        // Min time by ranks
    MAX,            // Max time by ranks
    AVG,            // Avg time by ranks
    MAX_TIME_METRIC // Metric count
} Time_Metric;

typedef enum {
    PURE = 0,   // Communication time
    OVRLP,      // Communication + computation time
    COMP,       // Computation time
    MAX_TIME_ID // Size of times array
} Time_Id;

typedef struct {
    double     times [MAX_TIME_ID];
    size_t     offset[MAX_TIME_ID];
} Timing;

extern double *all_times;

/* STRING FOR OUTPUT    */
extern char aux_string[out_fields*ow_format];

/* FORMAT FOR OUTPUT    */
extern char format    [out_fields*7];

/* I/O unit                  */
extern FILE* unit;

/* Error message headers*/

extern int err_flag;

extern void*  AUX;
extern size_t AUX_LEN;

extern int    do_nonblocking;
extern double tCPU;    /* CPU time for overlap benchmarks */
extern double MFlops;

extern int IMB_internal_barrier;

#define TOL 1.e-2

#define IMB_Assert(expr)  assert(expr)

#define IMB_i_alloc(type, B, Len, where )                       \
    {                                                           \
        IMB_Assert(Len>0);                                      \
        (B) = (type*) IMB_v_alloc(sizeof(type)*(Len), where);   \
    }

void IMB_Barrier(MPI_Comm comm);
#define IMB_do_n_barriers(comm, iter)       \
    {                                       \
        int _ii;                            \
        for (_ii = 0; _ii < iter; ++_ii)    \
        {                                   \
            if (IMB_internal_barrier)       \
            {                               \
                IMB_Barrier(comm);          \
            } else {                        \
                MPI_Barrier(comm);          \
            }                               \
        }                                   \
    }                                       \

#ifdef WIN_IMB
#define IMB_strcasecmp(s1, s2)  stricmp( (s1), (s2))
#else /*linux*/
#define IMB_strcasecmp(s1, s2)  strcasecmp( (s1), (s2))
#endif /*WIN_IMB*/

#endif /*#ifndef _DECLARE_H*/

#ifdef USE_MPI_INIT_THREAD
extern int mpi_thread_desired;
extern int mpi_thread_environment;
#endif /*USE_MPI_INIT_THREAD*/
