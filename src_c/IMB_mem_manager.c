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
    
 File: IMB_mem_manager.c 

 Implemented functions: 

 IMB_v_alloc;
 IMB_i_alloc;
 IMB_alloc_buf;
 IMB_alloc_aux;
 IMB_free_aux;
 IMB_v_free;
 IMB_ass_buf;
 IMB_set_buf;
 IMB_init_pointers;
   IMB 3.1 << 
 IMB_init_buffers_iter;
   >> IMB 3.1 
 IMB_free_all;
 IMB_del_s_buf;
 IMB_del_r_buf;

 ***************************************************************************/

#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_prototypes.h"


#include <limits.h> /* for INT_MAX declaration*/
#include <stdint.h>

#if defined(GPU_ENABLE) && defined(MPI1)
#include "IMB_gpu_common.h"
#define IMB_ALLOC(buff, size, where)                                  \
    do {                                                              \
        if (c_info->mem_alloc_type != MAT_CPU) {                      \
            buff = IMB_gpu_backend.IMB_gpu_alloc(size, where, c_info->mem_alloc_type);\
        }                                                             \
        else {                                                        \
            buff = IMB_v_alloc(size, where);                          \
        }                                                             \
    } while (0);

#define IMB_FREE(buff)                                 \
    do {                                               \
        if (c_info->mem_alloc_type != MAT_CPU) {       \
            IMB_gpu_backend.IMB_gpu_free(buff, c_info->mem_alloc_type);\
        }                                              \
        else {                                         \
            IMB_v_free(buff);                          \
        }                                              \
    } while (0);

#define IMB_ASSIGN(buf, rank, pos1, pos2, value)          \
    do {                                                  \
        if (c_info->mem_alloc_type != MAT_CPU) {          \
            IMB_gpu_backend.IMB_gpu_ass_buf(buf, rank, pos1, pos2, value);\
        }                                                 \
        else {                                            \
            IMB_ass_buf(buf, rank, pos1, pos2, value);    \
        }                                                 \
    } while (0);
#else // !GPU_ENABLE or !MPI1
#define IMB_ALLOC(buff, size, where)     \
    do {                                 \
        buff = IMB_v_alloc(size, where); \
    } while (0);

#define IMB_FREE(buff)    \
    do {                  \
        IMB_v_free(buff); \
    } while (0);

#define IMB_ASSIGN(buf, rank, pos1, pos2, value)   \
    do {                                           \
        IMB_ass_buf(buf, rank, pos1, pos2, value); \
    } while (0);

#endif // defined(GPU_ENABLE) && defined(MPI1)

static int asize = (int) sizeof(assign_type);

void* IMB_v_alloc(size_t size, char* where) {
/*

                      Allocates void* memory

Input variables:

-size                 (type size_t)
                      #bytes to allocate

-where                (type char*)
                      Comment (marker for calling place)

Return value          (type void*)
                      Allocated pointer

*/
    int mpi_error;
    void *ptr = NULL;
    if (size < sizeof(size_t)) {
        size = sizeof(size_t);
    }
    mpi_error = MPI_Alloc_mem(size, MPI_INFO_NULL, (void *)&ptr);
    if ((mpi_error != MPI_SUCCESS) || (ptr == NULL)) {
        printf("Memory allocation failed. code position: %s. tried to alloc."
#ifdef WIN_IMB
               " %I64u bytes\n",
#else
               " %lu bytes\n",
#endif
        where, size);
    }
#ifdef DEBUG
    if (dbg_file)
        fprintf(dbg_file, "alloc %p %s\n", ptr, where);
#endif
    num_alloc++;
    return ptr;
}

/***************************************************************************/
void IMB_alloc_buf(struct comm_info* c_info, char* where, size_t s_len,
                   size_t r_len) {
/*

                      Allocates send/recv buffers for message passing

Input variables:

-where                (type char*)
                      Comment (marker for calling place)


-s_len                (type int)
                      Send buffer length (bytes)

-r_len                (type int)
                      Recv buffer length (bytes)

In/out variables:

-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information

                      Send/Recv buffer components get allocated

*/
    s_len = max(1, s_len);
    r_len = max(1, r_len);

    if (c_info->s_alloc < s_len) {
        size_t size;
        IMB_FREE((void**)&c_info->s_buffer);
        size = s_len * ((size_t)c_info->size_scale);
        IMB_ALLOC(c_info->s_buffer, size, where);
        c_info->s_alloc = size / ((size_t)c_info->size_scale);
        c_info->s_data = (assign_type*)c_info->s_buffer;
    }

    if (c_info->r_alloc < r_len) {
        size_t size;
        IMB_FREE((void**)&c_info->r_buffer);
        size = r_len * ((size_t)c_info->size_scale);
        IMB_ALLOC(c_info->r_buffer, size, where);
        c_info->r_alloc = size / ((size_t)c_info->size_scale);
        c_info->r_data = (assign_type*)c_info->r_buffer;
    }
}



/***************************************************************************/
void IMB_alloc_aux(size_t L, char* where) {
/*

                      Allocates global auxiliary memory AUX

Input variables:

-L                    (type int)
                      #Bytes to allocate

-where                (type char*)
                      Comment (marker for calling place)

*/
    L += asize;
    if (AUX_LEN < L) {
        if (AUX_LEN > 0)
            IMB_v_free((void**)&AUX);

        AUX = IMB_v_alloc(L, where);
        AUX_LEN = AUX ? L : 0;
    }
}



/***************************************************************************/
void IMB_free_aux() {
/*

                      Free-s global auxiliary memory AUX

*/
    if (AUX_LEN > 0) {
        IMB_v_free((void**)&AUX); AUX_LEN = 0;
    }
}

void IMB_v_free(void **B) {
/*

                      Free-s memory

In/out variables:

-B                    (type void**)
                      (*B) will be free-d

*/
    if (*B) {
#ifdef DEBUG
        if (dbg_file)
            fprintf(dbg_file, "delete %p \n", *B);
#endif
        MPI_Free_mem(*B);
        num_free++;
    }

    *B = NULL;
}


/***************************************************************************/
void IMB_ass_buf(void* buf, int rank, size_t pos1, 
                 size_t pos2, int value) {
/*

                      Assigns values to a buffer

Input variables:

-rank                 (type int)
                      Rank of calling process

-pos1                 (type int)
-pos2                 (type int)
                      Assignment between byte positions pos1, pos2

-value                (type int)
                      1/0 for non-zero (defined in IMB_settings.h)/ zero value

In/out variables:

-buf                  (type void*)
                      Values assigned within given positions

*/
    if (pos2 >= pos1) {
        size_t a_pos1, a_pos2, i, j;
        a_pos1 = pos1 / asize;
        a_pos2 = pos2 / asize;

        if (value)
            for (i = a_pos1, j = 0; i <= a_pos2; i++, j++)
                ((assign_type *)buf)[j] = BUF_VALUE(rank, i);
        else
            for (i = a_pos1, j = 0; i <= a_pos2; i++, j++)
                ((assign_type *)buf)[j] = 0.;

        if (a_pos1*asize != pos1) {
            void* xx = (void*)(((char*)buf) + pos1 - a_pos1*asize);
            memmove(buf, xx, pos2 - pos1 + 1);
        }
    } /*if( pos2>= pos1 )*/
}



/***************************************************************************/
void IMB_set_buf(struct comm_info* c_info, int selected_rank, size_t s_pos1, 
                 size_t s_pos2, size_t r_pos1, size_t r_pos2) {
/*

                      Sets Send/Recv buffers for a selected rank
                      (by call to => IMB_ass_buf)

Input variables:

-selected_rank        (type int)
                      Relevant process rank
                      (Can be different from local rank: for checking purposes)

-s_pos1               (type int)
-s_pos2               (type int)
                      s_pos1 .. s_pos2 positions for send buffer

-r_pos1               (type int)
-r_pos2               (type int)
                      r_pos1 .. r_pos2 positions for recv buffer

In/out variables:

-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information

                      Corresponding buffer components are assigned values

*/
/*
Sets c_info->s_buffer/c_info->r_buffer int byte positions
s_pos1..s_pos2/r_pos1..r_pos2
Values are taken for "selected_rank"
Checks right allocation.
*/
    size_t s_len, r_len;

    s_len = (max(s_pos2 - s_pos1, 0) / asize + 1)*asize;
    r_len = (max(r_pos2 - r_pos1, 0) / asize + 1)*asize;

    IMB_alloc_buf(c_info, "set_buf 1", s_len, r_len);

    if (s_pos2 >= s_pos1)
        IMB_ASSIGN(c_info->s_buffer, selected_rank, s_pos1, s_pos2, 1);

    if (r_pos2 >= r_pos1)
        IMB_ASSIGN(c_info->r_buffer, selected_rank, r_pos1, r_pos2, 0);
}


/***************************************************************************/
void IMB_init_pointers(struct comm_info *c_info ) {
/*

                      Initializes pointer components of comm_info

In/out variables:

-c_info               (type struct comm_info *)
                      Collection of all base data for MPI;
                      see [1] for more information

                      Corresponding pointer components are initialized

*/
/********************************************************************


---------------------------------------------------------------------
             VARIABLE |       TYPE        |   MEANING
---------------------------------------------------------------------
In/Out     : c_info   | struct comm_info* | see comm_info.h 
                      |                   | Pointers initialized
----------------------------------------------------------------------*/

    MPI_Comm_size(MPI_COMM_WORLD, &c_info->w_num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &c_info->w_rank);

#ifdef DEBUG
    dbgf_name = IMB_str("DBG_   ");
    sprintf(dbgf_name + 4, "%d", c_info->w_rank);
    dbg_file = fopen(dbgf_name, "w");
    unit = dbg_file;
#endif

    c_info->s_data_type = MPI_BYTE;  /* DATA TYPE of SEND    BUFFER    */
    c_info->r_data_type = MPI_BYTE;  /* DATA TYPE of RECEIVE BUFFER    */

    c_info->op_type = MPI_SUM;   /* OPERATION TYPE IN Allred       */
    c_info->red_data_type = MPI_FLOAT; /* NOTE: NO 'CAST' CHECK IN. IBUF */

    c_info->size_scale   = 1;
    c_info->zero_size    = 1;
    c_info->contig_type  = CT_BASE;
    c_info->communicator = MPI_COMM_NULL;

    /* Auxiliary space */
    IMB_i_alloc(int, c_info->g_ranks, c_info->w_num_procs, "Init_Pointers 1");
    IMB_i_alloc(int, c_info->g_sizes, c_info->w_num_procs, "Init_Pointers 2");

#if (defined MPI1 || defined NBC)
    IMB_i_alloc(int, c_info->sndcnt, c_info->w_num_procs, "Init_Pointers 3");
    IMB_i_alloc(int, c_info->sdispl, c_info->w_num_procs, "Init_Pointers 4");

    IMB_i_alloc(int, c_info->reccnt, c_info->w_num_procs, "Init_Pointers 5");
    IMB_i_alloc(int, c_info->rdispl, c_info->w_num_procs, "Init_Pointers 6");
#else
    c_info->sndcnt = NULL;
    c_info->sdispl = NULL;
    c_info->reccnt = NULL;
    c_info->rdispl = NULL;
#endif

    all_times = NULL;
#ifdef CHECK
    all_defect = NULL;
#endif

#ifdef GPU_ENABLE
    c_info->mem_alloc_type = MAT_CPU;
#endif //GPU_ENABLE

    IMB_init_errhand(c_info);

}


/**********************************************************************/

static int has_root(const char* bname) {
    return bname &&
#if defined MPI1
        (!strcmp(bname, "Gather") ||
        !strcmp(bname, "Gatherv") ||
        !strcmp(bname, "Scatter") ||
        !strcmp(bname, "Scatterv") ||
        !strcmp(bname, "Bcast") ||
        !strcmp(bname, "Reduce"));
#elif defined NBC
        (!strcmp(bname, "Igather") ||
        !strcmp(bname, "Igatherv") ||
        !strcmp(bname, "Iscatter") ||
        !strcmp(bname, "Iscatterv") ||
        !strcmp(bname, "Ibcast") ||
        !strcmp(bname, "Ireduce") ||
        !strcmp(bname, "Igather_pure") ||
        !strcmp(bname, "Igatherv_pure") ||
        !strcmp(bname, "Iscatter_pure") ||
        !strcmp(bname, "Iscatterv_pure") ||
        !strcmp(bname, "Ibcast_pure") ||
        !strcmp(bname, "Ireduce_pure"));
#else
        0;
#endif /* MPI1 | NBC */
}

/* IMB 3.1 << */
/*
Major reconstruction of memory management for -off_cache flag
*/
void IMB_init_buffers_iter(struct comm_info* c_info, struct iter_schedule* ITERATIONS, 
                           struct Bench* Bmark, MODES BMODE, int iter, int size) {
/*

                      Initializes communications buffers (call set_buf)
                      Initializes iterations scheduling

Input variables:

-Bmark                (type struct Bench*)
                      (For explanation of struct Bench type:
                      describes all aspects of modes of a benchmark;
                      see [1] for more information)

                      Current benchmark

-BMODE                (type MODES)
                      aggregate / non aggregate

-iter                 (type int)
                      number of current iteration of message size loop

-size                 (type int)
                      Message size

In/out variables:

-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information

                      Communications buffers are allocated and assigned values

-ITERATIONS           (type struct iter_schedule*)
                      Adaptive number of iterations, out of cache scheduling are
                      setup if requested

*/
/* >> IMB 3.1  */
/* IMB 3.1 << */
    size_t s_len, r_len, s_alloc, r_alloc;
    int init_size, irep, i_s, i_r, x_sample;
    const int root_based = has_root(Bmark->name);


    x_sample = BMODE->AGGREGATE ? ITERATIONS->msgspersample : ITERATIONS->msgs_nonaggr;

    /* July 2002 fix V2.2.1: */
#if (defined EXT || defined MPIIO || RMA)
    if (Bmark->access == no) x_sample = ITERATIONS->msgs_nonaggr;
#endif

    ITERATIONS->n_sample = (size > 0)
                           ? max(1, min(ITERATIONS->overall_vol / size, x_sample))
                           : x_sample;

    Bmark->sample_failure = 0;

    init_size = max(size, asize);

    if (c_info->rank < 0)
        return;
    else {
        if (ITERATIONS->iter_policy == imode_off) {
            ITERATIONS->n_sample = x_sample = ITERATIONS->msgspersample;
        } else if ((ITERATIONS->iter_policy == imode_multiple_np) || 
                   (ITERATIONS->iter_policy == imode_auto && root_based)) {
            /* n_sample for benchmarks with uneven distribution of works
               must be greater or equal and multiple to num_procs.
               The formula below is a negative leg of hyperbola.
               It's moved and scaled relative to max message size
               and initial n_sample subject to multiple to num_procs.
               */
            double d_n_sample = ITERATIONS->msgspersample;
            int max_msg_size = 1 << c_info->max_msg_log;
            int tmp = (int)(d_n_sample*max_msg_size / (c_info->num_procs*init_size + max_msg_size) + 0.5);
            ITERATIONS->n_sample = x_sample = max(tmp - tmp%c_info->num_procs, c_info->num_procs);
        } /* else as is */
    }

    if (
#ifdef MPI1
        !strcmp(Bmark->name, "Alltoall") || !strcmp(Bmark->name, "Alltoallv")
#elif defined NBC // MPI1
        !strcmp(Bmark->name, "Ialltoall") || !strcmp(Bmark->name, "Ialltoall_pure")
        || !strcmp(Bmark->name, "Ialltoallv") || !strcmp(Bmark->name, "Ialltoallv_pure")
#else
        0
#endif // NBC // MPI1
        ) {
        s_len = (size_t)c_info->num_procs * (size_t)init_size;
        r_len = (size_t)c_info->num_procs * (size_t)init_size;
    } else if (
#ifdef MPI1
        !strcmp(Bmark->name, "Allgather") || !strcmp(Bmark->name, "Allgatherv")
        || !strcmp(Bmark->name, "Gather") || !strcmp(Bmark->name, "Gatherv")
#elif defined NBC
        !strcmp(Bmark->name, "Iallgather") || !strcmp(Bmark->name, "Iallgather_pure")
        || !strcmp(Bmark->name, "Iallgatherv") || !strcmp(Bmark->name, "Iallgatherv_pure")
        || !strcmp(Bmark->name, "Igather") || !strcmp(Bmark->name, "Igather_pure")
        || !strcmp(Bmark->name, "Igatherv") || !strcmp(Bmark->name, "Igatherv_pure")
#else // MPI1 // NBC
        0
#endif // MPI1 // NBC
        ) {
        s_len = (size_t)init_size;
        r_len = (size_t)c_info->num_procs * (size_t)init_size;
    } else if (!strcmp(Bmark->name, "Exchange")) {
        s_len = 2 * (size_t)init_size;
        r_len = (size_t)init_size;
    } else if (
#ifdef MPI1
        !strcmp(Bmark->name, "Scatter") || !strcmp(Bmark->name, "Scatterv")
#elif defined NBC // MPI1
        !strcmp(Bmark->name, "Iscatter") || !strcmp(Bmark->name, "Iscatter_pure")
        || !strcmp(Bmark->name, "Iscatterv") || !strcmp(Bmark->name, "Iscatterv_pure")
#else // NBC // MPI1
        0
#endif // NBC // MPI1
        ) {
        s_len = (size_t)c_info->num_procs * (size_t)init_size;
        r_len = (size_t)init_size;
    } else if (!strcmp(Bmark->name, "Barrier") || /*!strcmp(Bmark->name,"Window") ||*/ !strcmp(Bmark->name, "Open_Close")) {
        s_len = r_len = 0;
    } else if (!strcmp(Bmark->name, "Exchange_put") || !strcmp(Bmark->name, "Exchange_get")) {
        s_len = 2 * (size_t)init_size;
        r_len = 2 * (size_t)init_size;
    } else if (!strcmp(Bmark->name, "Compare_and_swap")) {
        /* Compare_and_swap operations require 3 buffers, so allocate space for compare
         * buffers in our r_buffer */
        s_len = (size_t)init_size;
        r_len = 3 * (size_t)init_size;
    } else {
        s_len = r_len = (size_t)init_size;
    }

    /*===============================================*/
    /* the displ is declared as int by MPI1 standard
       If c_info->num_procs*init_size  exceed INT_MAX value there is no way to run this sample
       */

    if (s_len > INT_MAX || r_len > INT_MAX) {
        Bmark->sample_failure = SAMPLE_FAILED_INT_OVERFLOW;
        return;
    }
    /*===============================================*/

    /* IMB 3.1: new memory management for -off_cache */
    if (BMODE->type == Sync) {
        ITERATIONS->use_off_cache = 0;
        ITERATIONS->n_sample = x_sample;
    } else {
#ifdef MPIIO
        ITERATIONS->use_off_cache = 0;
#else  
        ITERATIONS->use_off_cache = ITERATIONS->off_cache;
#endif  
        if (ITERATIONS->off_cache) {
            if (ITERATIONS->cache_size > 0) {
                size_t cls = (size_t)ITERATIONS->cache_line_size;
                size_t ofs = ((s_len + cls - 1) / cls + 1) * cls;
                ITERATIONS->s_offs = ofs;
                ITERATIONS->s_cache_iter = min(ITERATIONS->n_sample, (2 * ITERATIONS->cache_size*CACHE_UNIT + ofs - 1) / ofs);
                ofs = ((r_len + cls - 1) / cls + 1)*cls;
                ITERATIONS->r_offs = ofs;
                ITERATIONS->r_cache_iter = min(ITERATIONS->n_sample, (2 * ITERATIONS->cache_size*CACHE_UNIT + ofs - 1) / ofs);
            } else {
                ITERATIONS->s_offs = ITERATIONS->r_offs = 0;
                ITERATIONS->s_cache_iter = ITERATIONS->r_cache_iter = 1;
            }
        }
    }

#ifdef MPIIO
    s_alloc = s_len;
    r_alloc = r_len;
#else
    if (ITERATIONS->use_off_cache) {
        s_alloc = max(s_len, ITERATIONS->s_cache_iter*ITERATIONS->s_offs);
        r_alloc = max(r_len, ITERATIONS->r_cache_iter*ITERATIONS->r_offs);
    } else {
        s_alloc = s_len;
        r_alloc = r_len;
    }
#endif

    c_info->used_mem = 1.f*(s_alloc + r_alloc) / MEM_UNIT;

#ifdef DEBUG 
    {
        size_t mx, mu;

        mx = (size_t)MEM_UNIT*c_info->max_mem;
        mu = (size_t)MEM_UNIT*c_info->used_mem;

        DBG_I3("Got send / recv lengths; iters ", s_len, r_len, ITERATIONS->n_sample);
        DBG_I2("max  / used memory ", mx, mu);
        DBG_I2("send / recv offsets ", ITERATIONS->s_offs, ITERATIONS->r_offs);
        DBG_I2("send / recv cache iterations ", ITERATIONS->s_cache_iter, ITERATIONS->r_cache_iter);
        DBG_I2("send / recv buffer allocations ", s_alloc, r_alloc);
        DBGF_I2("Got send / recv lengths ", s_len, r_len);
        DBGF_I2("max  / used memory ", mx, mu);
        DBGF_I2("send / recv offsets ", ITERATIONS->s_offs, ITERATIONS->r_offs);
        DBGF_I2("send / recv cache iterations ", ITERATIONS->s_cache_iter, ITERATIONS->r_cache_iter);
        DBGF_I2("send / recv buffer allocations ", s_alloc, r_alloc);
    }
#endif

    if (c_info->used_mem > c_info->max_mem) {
        Bmark->sample_failure = SAMPLE_FAILED_MEMORY;
        return;
    }

    if (s_alloc > 0 && r_alloc > 0) {
        if (ITERATIONS->use_off_cache) {
            IMB_alloc_buf(c_info, "IMB_init_buffers_iter 1", s_alloc, r_alloc);
            IMB_set_buf(c_info, c_info->rank, 0, s_len - 1, 0, r_len - 1);

            for (irep = 1; irep < ITERATIONS->s_cache_iter; irep++) {
                i_s = irep % ITERATIONS->s_cache_iter;
                memcpy((void*)((char*)c_info->s_buffer + i_s * ITERATIONS->s_offs), c_info->s_buffer, s_len);
            }

            for (irep = 1; irep < ITERATIONS->r_cache_iter; irep++) {
                i_r = irep % ITERATIONS->r_cache_iter;
                memcpy((void*)((char*)c_info->r_buffer + i_r * ITERATIONS->r_offs), c_info->r_buffer, r_len);
            }
        } else {
            IMB_set_buf(c_info, c_info->rank, 0, s_alloc - 1, 0, r_alloc - 1);
        }
    }

    IMB_init_transfer(c_info, Bmark, size, (MPI_Aint)max(s_alloc, r_alloc));

    /* Determine #iterations if dynamic adaptation requested */
    if ((ITERATIONS->iter_policy == imode_dynamic) || (ITERATIONS->iter_policy == imode_auto && !root_based)) {
        double time[MAX_TIME_ID];
        int acc_rep_test, t_sample;
        int selected_n_sample = ITERATIONS->n_sample;

        memset(time, 0, MAX_TIME_ID * sizeof(*time));
        if (iter == 0 || BMODE->type == Sync) {
            ITERATIONS->n_sample_prev = ITERATIONS->msgspersample;
            if (c_info->n_lens > 0) {
                memset(ITERATIONS->numiters, 0, c_info->n_lens);
            }
        }

        /* first, run 1 iteration only */
        ITERATIONS->n_sample = 1;
#ifdef MPI1
        c_info->select_source = Bmark->select_source;
#endif
        Bmark->Benchmark(c_info, size, ITERATIONS, BMODE, &time[0]);

        time[1] = time[0];

#ifdef MPIIO
        if (Bmark->access != no) {
            MPI_ERRHAND(MPI_File_seek(c_info->fh, 0, MPI_SEEK_SET));

            if (Bmark->fpointer == shared) {
                MPI_ERRHAND(MPI_File_seek_shared(c_info->fh, 0, MPI_SEEK_SET));
            }
        }
#endif /*MPIIO*/

        MPI_Allreduce(&time[1], &time[0], 1, MPI_DOUBLE, MPI_MAX, c_info->communicator);

        { /* determine rough #repetitions for a run time of 1 sec */
            int rep_test = 1;
            if (time[0] < (1.0 / MSGSPERSAMPLE)) {
                rep_test = MSGSPERSAMPLE;
            }
            else if ((time[0] < 1.0)) {
                rep_test = (int)(1.0 / time[0] + 0.5);
            }

            MPI_Allreduce(&rep_test, &acc_rep_test, 1, MPI_INT, MPI_MAX, c_info->communicator);
        }

        ITERATIONS->n_sample = min(selected_n_sample, acc_rep_test);

        if (ITERATIONS->n_sample > 1) {
#ifdef MPI1
            c_info->select_source = Bmark->select_source;
#endif
            Bmark->Benchmark(c_info, size, ITERATIONS, BMODE, &time[0]);
            time[1] = time[0];
#ifdef MPIIO
            if (Bmark->access != no) {
                MPI_ERRHAND(MPI_File_seek(c_info->fh, 0, MPI_SEEK_SET));

                if (Bmark->fpointer == shared) {
                    MPI_ERRHAND(MPI_File_seek_shared(c_info->fh, 0, MPI_SEEK_SET));
                }
            }
#endif /*MPIIO*/

            MPI_Allreduce(&time[1], &time[0], 1, MPI_DOUBLE, MPI_MAX, c_info->communicator);
        }

        {
            float val = (float)(1 + ITERATIONS->secs / time[0]);
            t_sample = (time[0] > 1.e-8 && (val <= (float)0x7fffffff))
                ? (int)val
                : selected_n_sample;
        }

        if (c_info->n_lens > 0 && BMODE->type != Sync) {
            // check monotonicity with msg sizes 
            int i;
            for (i = 0; i < iter; i++) {
                t_sample = (c_info->msglen[i] < size)
                           ? min(t_sample, ITERATIONS->numiters[i])
                           : max(t_sample, ITERATIONS->numiters[i]);
            }
            ITERATIONS->n_sample = ITERATIONS->numiters[iter] = min(selected_n_sample, t_sample);
        } else {
            ITERATIONS->n_sample = min(selected_n_sample,
                min(ITERATIONS->n_sample_prev, t_sample));
        }

        MPI_Bcast(&ITERATIONS->n_sample, 1, MPI_INT, 0, c_info->communicator);

#ifdef DEBUG
        {
            int usec = time * 1000000;

            DBGF_I2("Checked time with #iters / usec ", acc_rep_test, usec);
            DBGF_I1("=> # samples, aligned with previous ", t_sample);
            DBGF_I1("final #samples ", ITERATIONS->n_sample);
        }
#endif
    } else { /*if( (ITERATIONS->iter_policy == imode_dynamic) || (ITERATIONS->iter_policy == imode_auto && !root_based) )*/
        double time[MAX_TIME_ID];
        Bmark->Benchmark(c_info, size, ITERATIONS, BMODE, &time[0]);
    }

    ITERATIONS->n_sample_prev = ITERATIONS->n_sample;

    /* >> IMB 3.1  */

}

/********************************************************************/

extern char *duplicated_benchmark_names[1000];

/* IMB 3.1 << */
void IMB_free_all(struct comm_info* c_info, struct Bench** P_BList, struct iter_schedule* ITERATIONS) {
/* >> IMB 3.1  */
/*

                      Free-s all allocated memory in c_info and P_Blist

In/out variables:

-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information

-P_BList              (type struct Bench**)
                      (For explanation of struct Bench type:
                      describes all aspects of modes of a benchmark;
                      see [1] for more information)

-ITERATIONS           (type struct iter_schedule*)
                      Iteration scheduling object

*/
    int i;
    for (i = 0; i < 1000; i++) {
        if (duplicated_benchmark_names[i] != NULL)
            free(duplicated_benchmark_names[i]);
        duplicated_benchmark_names[i] = NULL;
    }
    IMB_del_s_buf(c_info);
    IMB_del_r_buf(c_info);

    IMB_v_free((void**)&c_info->msglen);
    /* IMB 3.1 << */
    IMB_v_free((void**)&ITERATIONS->numiters);
    /* >> IMB 3.1  */

    IMB_v_free((void**)&c_info->g_sizes);
    IMB_v_free((void**)&c_info->g_ranks);

    IMB_v_free((void**)&c_info->sndcnt);
    IMB_v_free((void**)&c_info->sdispl);

    IMB_v_free((void**)&c_info->reccnt);
    IMB_v_free((void**)&c_info->rdispl);

    if (c_info->communicator != MPI_COMM_NULL &&
        c_info->communicator != MPI_COMM_SELF &&
        c_info->communicator != MPI_COMM_WORLD) {
        IMB_del_errhand(c_info);
        MPI_Comm_free(&c_info->communicator);
    }

    IMB_destruct_blist(P_BList);

#ifdef MPIIO
    IMB_free_file(c_info);
#endif
    if (all_times)
        IMB_v_free((void**)&all_times);

#ifdef CHECK
    if (all_defect)
        IMB_v_free((void**)&all_defect);

    if (AUX_LEN > 0) {
        IMB_v_free((void**)&AUX);
        AUX_LEN = 0; 
    }
#endif

#ifdef DEBUG
    IMB_v_free((void**)&dbgf_name);
    fclose(dbg_file);
#endif
}

void IMB_del_s_buf(struct comm_info* c_info ) {
/*

                      Deletes send buffer component of c_info

In/out variables:

-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information

*/
    if (c_info->s_alloc > 0) {
        IMB_FREE((void**)&c_info->s_buffer);
        c_info->s_alloc = 0;
        c_info->s_buffer = NULL;

    }
}

void IMB_del_r_buf(struct comm_info* c_info){
/*

                      Deletes recv buffer component of c_info

In/out variables:

-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information

*/
    if (c_info->r_alloc > 0) {
        IMB_FREE((void**)&c_info->r_buffer);
        c_info->r_alloc = 0;
        c_info->r_buffer = NULL;

    }
}
