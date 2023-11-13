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

 File: IMB_rma_atomic.c 

 Implemented functions: 

 IMB_rma_accumulate;
 IMB_rma_get_accumulate;
 IMB_rma_fetch_and_op;
 IMB_rma_compare_and_swap;
 
 ***************************************************************************/

/*-----------------------------------------------------------*/

#include "IMB_declare.h"
#include "IMB_benchmark.h"
#include "IMB_prototypes.h"

void IMB_rma_accumulate(struct comm_info* c_info, int size,
                        struct iter_schedule* iterations,
                        MODES run_mode, double* time) {
    double res_time = -1.;
    Type_Size s_size, r_size;
    int s_num = 0,
        r_num = 0;
    /* IMB 3.1 << */
    int r_off;
    int i;
    int root = c_info->pair1;

    if (c_info->rank < 0) {
        *time = res_time;
        return;
    }

    MPI_Type_size(c_info->red_data_type, &s_size);
    s_num = size / s_size;
    r_size = s_size;
    r_num = s_num;
    r_off = iterations->r_offs / r_size;

    for (i = 0; i < N_BARR; i++)
        MPI_Barrier(c_info->communicator);

    if (c_info->rank == c_info->pair0) {
        MPI_Win_lock(MPI_LOCK_SHARED, root, 0, c_info->WIN);
        if (run_mode->AGGREGATE) {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++) {
                MPI_ERRHAND(MPI_Accumulate((char*)c_info->s_buffer + i%iterations->s_cache_iter*iterations->s_offs,
                                           s_num, c_info->red_data_type, root,
                                           i%iterations->r_cache_iter*r_off, r_num,
                                           c_info->red_data_type, c_info->op_type, c_info->WIN));
            }
            MPI_ERRHAND(MPI_Win_flush(root, c_info->WIN));
            res_time = (MPI_Wtime() - res_time) / iterations->n_sample;
        } else if (!run_mode->AGGREGATE) {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++) {
                MPI_ERRHAND(MPI_Accumulate((char*)c_info->s_buffer + i%iterations->s_cache_iter*iterations->s_offs,
                                           s_num, c_info->red_data_type, root,
                                           i%iterations->r_cache_iter*r_off, r_num,
                                           c_info->red_data_type, c_info->op_type, c_info->WIN));

                MPI_ERRHAND(MPI_Win_flush(root, c_info->WIN));
            }
            res_time = (MPI_Wtime() - res_time) / iterations->n_sample;
        }
        MPI_Win_unlock(root, c_info->WIN);
    }
    MPI_Barrier(c_info->communicator);

    *time = res_time;
    return;
}


void IMB_rma_get_accumulate(struct comm_info* c_info, int size,
                            struct iter_schedule* iterations,
                            MODES run_mode, double* time) {
    double res_time = -1.;
    Type_Size s_size, r_size;
    int s_num = 0,
        r_num = 0;
    int r_off;
    int i;
    int root = c_info->pair1;

    if (c_info->rank < 0) {
        *time = res_time;
        return;
    }

    MPI_Type_size(c_info->red_data_type, &s_size);
    s_num = size / s_size;
    r_size = s_size;
    r_num = s_num;
    r_off = iterations->r_offs / r_size;

    for (i = 0; i < N_BARR; i++)
        MPI_Barrier(c_info->communicator);

    if (c_info->rank == c_info->pair0) {
        MPI_Win_lock(MPI_LOCK_SHARED, root, 0, c_info->WIN);
        if (run_mode->AGGREGATE) {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++) {
                MPI_ERRHAND(MPI_Get_accumulate((char*)c_info->s_buffer + i%iterations->s_cache_iter*iterations->s_offs,
                                               s_num, c_info->red_data_type,
                                               (char*)c_info->r_buffer + i%iterations->r_cache_iter*iterations->r_offs,
                                               r_num, c_info->red_data_type,
                                               root, i%iterations->r_cache_iter*r_off, r_num,
                                               c_info->red_data_type, c_info->op_type, c_info->WIN));
            }
            MPI_ERRHAND(MPI_Win_flush(root, c_info->WIN));
            res_time = (MPI_Wtime() - res_time) / iterations->n_sample;
        } else if (!run_mode->AGGREGATE) {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++) {
                MPI_ERRHAND(MPI_Get_accumulate((char*)c_info->s_buffer + i%iterations->s_cache_iter*iterations->s_offs,
                                               s_num, c_info->red_data_type,
                                               (char*)c_info->r_buffer + i%iterations->r_cache_iter*iterations->r_offs,
                                               r_num, c_info->red_data_type,
                                               root, i%iterations->r_cache_iter*r_off, r_num,
                                               c_info->red_data_type, c_info->op_type, c_info->WIN));

                MPI_ERRHAND(MPI_Win_flush(root, c_info->WIN));
            }
            res_time = (MPI_Wtime() - res_time) / iterations->n_sample;
        }
        MPI_Win_unlock(root, c_info->WIN);
    }
    MPI_Barrier(c_info->communicator);

    *time = res_time;
    return;
}

void IMB_rma_fetch_and_op(struct comm_info* c_info, int size,
                          struct iter_schedule* iterations,
                          MODES run_mode, double* time) {
    double res_time = -1.;
    Type_Size r_size;
    int r_off;
    int i;
    int root = c_info->pair1;

    if (c_info->rank < 0) {
        *time = res_time;
        return;
    }

    MPI_Type_size(c_info->red_data_type, &r_size);
    r_off = iterations->r_offs / r_size;

    for (i = 0; i < N_BARR; i++)
        MPI_Barrier(c_info->communicator);

    if (c_info->rank == c_info->pair0) {
        MPI_Win_lock(MPI_LOCK_SHARED, root, 0, c_info->WIN);
        if (run_mode->AGGREGATE) {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++) {
                MPI_ERRHAND(MPI_Fetch_and_op((char*)c_info->s_buffer + i%iterations->s_cache_iter*iterations->s_offs,
                                             (char*)c_info->r_buffer + i%iterations->r_cache_iter*iterations->r_offs,
                                             c_info->red_data_type, root,
                                             i%iterations->r_cache_iter*r_off, c_info->op_type, c_info->WIN));
            }
            MPI_ERRHAND(MPI_Win_flush(root, c_info->WIN));
            res_time = (MPI_Wtime() - res_time) / iterations->n_sample;
        } else if (!run_mode->AGGREGATE) {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++) {
                MPI_ERRHAND(MPI_Fetch_and_op((char*)c_info->s_buffer + i%iterations->s_cache_iter*iterations->s_offs,
                                             (char*)c_info->r_buffer + i%iterations->r_cache_iter*iterations->r_offs,
                                             c_info->red_data_type, root,
                                             i%iterations->r_cache_iter*r_off, c_info->op_type, c_info->WIN));

                MPI_ERRHAND(MPI_Win_flush(root, c_info->WIN));
            }
            res_time = (MPI_Wtime() - res_time) / iterations->n_sample;
        }
        MPI_Win_unlock(root, c_info->WIN);
    }
    MPI_Barrier(c_info->communicator);

    *time = res_time;
    return;
}

void IMB_rma_compare_and_swap(struct comm_info* c_info, int size,
                              struct iter_schedule* iterations,
                              MODES run_mode, double* time) {
    double res_time = -1.;
    int root = c_info->pair1;
    int s_size;
    int i;
    void *comp_b, *orig_b, *res_b;
    MPI_Datatype data_type = MPI_INT;

    if (c_info->rank < 0) {
        *time = res_time;
        return;
    }

    MPI_Type_size(data_type, &s_size);
    for (i = 0; i < N_BARR; i++)
        MPI_Barrier(c_info->communicator);


    if (c_info->rank == c_info->pair0) {
        /* use r_buffer for all buffers required by compare_and_swap, because
         * on all ranks r_buffer is zero-initialized in IMB_set_buf function */
        orig_b = (char*)c_info->r_buffer + s_size * 2;
        comp_b = (char*)c_info->r_buffer + s_size;
        res_b = c_info->r_buffer;

        MPI_Win_lock(MPI_LOCK_SHARED, root, 0, c_info->WIN);
        if (run_mode->AGGREGATE) {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++) {
                MPI_ERRHAND(MPI_Compare_and_swap((char*)orig_b + i%iterations->r_cache_iter*iterations->r_offs,
                                                 (char*)comp_b + i%iterations->r_cache_iter*iterations->r_offs,
                                                 (char*)res_b + i%iterations->r_cache_iter*iterations->r_offs,
                                                 data_type, root, i%iterations->r_cache_iter*iterations->r_offs,
                                                 c_info->WIN));
            }
            MPI_ERRHAND(MPI_Win_flush(root, c_info->WIN));
            res_time = (MPI_Wtime() - res_time) / iterations->n_sample;
        } else if (!run_mode->AGGREGATE) {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++) {
                MPI_ERRHAND(MPI_Compare_and_swap((char*)orig_b + i%iterations->s_cache_iter*iterations->s_offs,
                                                 (char*)comp_b + i%iterations->s_cache_iter*iterations->s_offs,
                                                 (char*)res_b + i%iterations->r_cache_iter*iterations->r_offs,
                                                 data_type, root, i%iterations->r_cache_iter*iterations->r_offs,
                                                 c_info->WIN));

                MPI_ERRHAND(MPI_Win_flush(root, c_info->WIN));
            }
            res_time = (MPI_Wtime() - res_time) / iterations->n_sample;
        }
        MPI_Win_unlock(root, c_info->WIN);
    }
    MPI_Barrier(c_info->communicator);

    *time = res_time;
    return;
}

