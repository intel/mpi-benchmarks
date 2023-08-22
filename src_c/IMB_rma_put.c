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
    
 File: IMB_rma_put.c 

 Implemented functions: 

 IMB_rma_single_put;
 IMB_rma_put_all;
 IMB_rma_put_local;
 IMB_rma_put_all_local;
 IMB_rma_exchange_put;
 IMB_rma_passive_put;
 
 ***************************************************************************/

#include "IMB_declare.h"
#include "IMB_benchmark.h"
#include "IMB_prototypes.h"


/* Unidirectional and bidirectional put: communication is done
 * between two processes only. */
void IMB_rma_single_put(struct comm_info* c_info, int size,
                        struct iter_schedule* iterations,
                        MODES run_mode, double* time) {
    double res_time = -1.;
    int target = -1;
    int sender = 0;
    Type_Size s_size;
    int s_num = 0;
    int i;
#ifdef CHECK 
    int asize = (int) sizeof(assign_type);
    char *recv = (char *)c_info->r_buffer;
    defect = 0;
#endif

    if (c_info->rank == c_info->pair0) {
        target = c_info->pair1;
        sender = 1;
    } else if (c_info->rank == c_info->pair1) {
        target = c_info->pair0;
        if (run_mode->BIDIR) {
            /* pair1 acts as origin in bidirectional mode only */
            sender = 1;
        }
    } else if (c_info->rank < 0) {
        *time = res_time;
        return;
    }

    MPI_Type_size(c_info->s_data_type, &s_size);
    s_num = size / s_size;

    for (i = 0; i < N_BARR; i++)
        MPI_Barrier(c_info->communicator);

    if (sender) {
        MPI_Win_lock(MPI_LOCK_SHARED, target, 0, c_info->WIN);
        if (run_mode->AGGREGATE) {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++) {
                MPI_ERRHAND(MPI_Put((char*)c_info->s_buffer + i%iterations->s_cache_iter*iterations->s_offs,
                                    s_num, c_info->s_data_type, target,
                                    i%iterations->r_cache_iter*iterations->r_offs,
                                    s_num, c_info->r_data_type, c_info->WIN));
            }
            MPI_ERRHAND(MPI_Win_flush(target, c_info->WIN));
            res_time = (MPI_Wtime() - res_time) / iterations->n_sample;
        } else if (!run_mode->AGGREGATE) {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++) {
                MPI_ERRHAND(MPI_Put((char*)c_info->s_buffer + i%iterations->s_cache_iter*iterations->s_offs,
                                    s_num, c_info->s_data_type, target,
                                    i%iterations->r_cache_iter*iterations->r_offs,
                                    s_num, c_info->r_data_type, c_info->WIN));
                MPI_ERRHAND(MPI_Win_flush(target, c_info->WIN));
            }
            res_time = (MPI_Wtime() - res_time) / iterations->n_sample;
        }
        MPI_Win_unlock(target, c_info->WIN);
    }
    /* Synchronize target and origin processes */
    MPI_Barrier(c_info->communicator);

#ifdef CHECK
    if (!sender || run_mode->BIDIR) {
        for (i = 0; i < ITER_MIN(iterations); i++) {
            CHK_DIFF("MPI_Put", c_info, (void*)(recv + i%iterations->r_cache_iter*iterations->r_offs),
                     0, size, size, asize, put, 0, iterations->n_sample, i, target, &defect);
        }
    }
#endif     
    *time = res_time;
    return;
}

/* Implements "One_put_all" and "all_put_all" benchmarks:
 * run_mode Collective corresponds to "All_put_all",
 * run_mode MultPassiveTransfer corresponds to "One_put_all"
 * */
void IMB_rma_put_all(struct comm_info* c_info, int size,
                     struct iter_schedule* iterations,
                     MODES run_mode, double* time) {
    double res_time = -1.;
    int target = 0;
    int peer = 0;
    int sender = 0;
    Type_Size s_size;
    int s_num = 0;
    int i;

    if (c_info->rank < 0) {
        *time = res_time;
        return;
    }

    if (c_info->rank == 0 || run_mode->type == Collective)
        sender = 1;

    MPI_Type_size(c_info->s_data_type, &s_size);
    s_num = size / s_size;

    for (i = 0; i < N_BARR; i++)
        MPI_Barrier(c_info->communicator);

    if (sender) {
        MPI_Win_lock_all(0, c_info->WIN);

        res_time = MPI_Wtime();
        for (i = 0; i < iterations->n_sample; i++) {
            for (peer = 0; peer < c_info->num_procs; peer++) {
                /* choose different target for each process to avoid congestion */
                target = (peer + c_info->rank) % c_info->num_procs;
                if (target == c_info->rank)
                    continue; /* do not put to itself*/
                MPI_ERRHAND(MPI_Put((char*)c_info->s_buffer + i%iterations->s_cache_iter*iterations->s_offs,
                                    s_num, c_info->s_data_type, target,
                                    i%iterations->r_cache_iter*iterations->r_offs,
                                    s_num, c_info->r_data_type, c_info->WIN));
            }
        }
        MPI_ERRHAND(MPI_Win_flush_all(c_info->WIN));
        res_time = (MPI_Wtime() - res_time) / iterations->n_sample;

        MPI_Win_unlock_all(c_info->WIN);
    }
    /* Synchronize origin and target processes */
    MPI_Barrier(c_info->communicator);

    *time = res_time;
    return;
}

/* Implements "Put_local" benchmark. One process puts some data
 * to the other and make sure of completion by MPI_Win_flush_local call
 * */
void IMB_rma_put_local(struct comm_info* c_info, int size,
                       struct iter_schedule* iterations,
                       MODES run_mode, double* time) {
    double res_time = -1.;
    Type_Size s_size;
    int s_num = 0;
    int i;

    if (c_info->rank < 0) {
        *time = res_time;
        return;
    }

    MPI_Type_size(c_info->s_data_type, &s_size);
    s_num = size / s_size;

    for (i = 0; i < N_BARR; i++)
        MPI_Barrier(c_info->communicator);

    if (c_info->rank == c_info->pair0) {
        MPI_Win_lock(MPI_LOCK_SHARED, c_info->pair1, 0, c_info->WIN);
        if (run_mode->AGGREGATE) {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++) {
                MPI_ERRHAND(MPI_Put((char*)c_info->s_buffer + i%iterations->s_cache_iter*iterations->s_offs,
                                    s_num, c_info->s_data_type, c_info->pair1,
                                    i%iterations->r_cache_iter*iterations->r_offs,
                                    s_num, c_info->r_data_type, c_info->WIN));
            }
            MPI_ERRHAND(MPI_Win_flush_local(c_info->pair1, c_info->WIN));
            res_time = (MPI_Wtime() - res_time) / iterations->n_sample;
        } else if (!run_mode->AGGREGATE) {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++) {
                MPI_ERRHAND(MPI_Put((char*)c_info->s_buffer + i%iterations->s_cache_iter*iterations->s_offs,
                                    s_num, c_info->s_data_type, c_info->pair1,
                                    i%iterations->r_cache_iter*iterations->r_offs,
                                    s_num, c_info->r_data_type, c_info->WIN));
                MPI_ERRHAND(MPI_Win_flush_local(c_info->pair1, c_info->WIN));
            }
            res_time = (MPI_Wtime() - res_time) / iterations->n_sample;
        }
        MPI_Win_unlock(c_info->pair1, c_info->WIN);
    }
    /* Synchronize target and origin processes */
    MPI_Barrier(c_info->communicator);

    *time = res_time;
    return;
}

/* Implements "put_all_local" benchmark. One process puts some data
 * to all other processes and make sure of completion by
 * MPI_Win_flush_local_all call
 * */
void IMB_rma_put_all_local(struct comm_info* c_info, int size,
                           struct iter_schedule* iterations,
                           MODES run_mode, double* time) {
    double res_time = -1.;
    int target = 0;
    int peer = 0;
    Type_Size s_size;
    int s_num = 0;
    int i;

    if (c_info->rank < 0) {
        *time = res_time;
        return;
    }

    MPI_Type_size(c_info->s_data_type, &s_size);
    s_num = size / s_size;

    for (i = 0; i < N_BARR; i++)
        MPI_Barrier(c_info->communicator);

    if (c_info->rank == 0) {
        MPI_Win_lock_all(0, c_info->WIN);

        res_time = MPI_Wtime();
        for (i = 0; i < iterations->n_sample; i++) {
            for (peer = 0; peer < c_info->num_procs; peer++) {
                /* choose different target for each process to avoid congestion */
                target = (peer + c_info->rank) % c_info->num_procs;
                if (target == c_info->rank)
                    continue; /* do not put to itself*/

                MPI_ERRHAND(MPI_Put((char*)c_info->s_buffer + i%iterations->s_cache_iter*iterations->s_offs,
                                    s_num, c_info->s_data_type, target,
                                    i%iterations->r_cache_iter*iterations->r_offs,
                                    s_num, c_info->r_data_type, c_info->WIN));
            }
        }
        MPI_ERRHAND(MPI_Win_flush_local_all(c_info->WIN));
        res_time = (MPI_Wtime() - res_time) / iterations->n_sample;

        MPI_Win_unlock_all(c_info->WIN);
    }
    /* Synchronize origin and target processes */
    MPI_Barrier(c_info->communicator);

    *time = res_time;
    return;
}

/* Implements "Exchange_put" benchmark. Each process puts some data
 * to two neighbor processes
 * */
void IMB_rma_exchange_put(struct comm_info* c_info, int size,
                          struct iter_schedule* iterations,
                          MODES run_mode, double* time) {
    double res_time = -1.;
    int left = -1;
    int right = -1;
    Type_Size s_size;
    int s_num = 0;
    int i;
#ifdef CHECK 
    int asize = (int) sizeof(assign_type);
    char *recv = (char *)c_info->r_buffer;
    defect = 0;
#endif

    if (c_info->rank < 0) {
        *time = res_time;
        return;
    }

    MPI_Type_size(c_info->s_data_type, &s_size);
    s_num = size / s_size;

    left = (c_info->rank == 0) ? c_info->num_procs - 1 : c_info->rank - 1;
    right = (c_info->rank + 1) % c_info->num_procs;

    for (i = 0; i < N_BARR; i++)
        MPI_Barrier(c_info->communicator);

    if (left != right) {
        MPI_Win_lock(MPI_LOCK_SHARED, left, 0, c_info->WIN);
        MPI_Win_lock(MPI_LOCK_SHARED, right, 0, c_info->WIN);
    } else {
        /* Just two processes in the chain. Both messages will come
         * to the one peer */
        MPI_Win_lock(MPI_LOCK_SHARED, left, 0, c_info->WIN);
    }

    res_time = MPI_Wtime();
    for (i = 0; i < iterations->n_sample; i++) {
        MPI_ERRHAND(MPI_Put((char*)c_info->s_buffer + i%iterations->s_cache_iter*iterations->s_offs,
                            s_num, c_info->s_data_type, left, 
                            i%iterations->r_cache_iter*iterations->r_offs,
                            s_num, c_info->r_data_type, c_info->WIN));

        MPI_ERRHAND(MPI_Put((char*)c_info->s_buffer + size + i%iterations->s_cache_iter*iterations->s_offs,
                            s_num, c_info->s_data_type, right,
                            i%iterations->r_cache_iter*iterations->r_offs + size,
                            s_num, c_info->r_data_type, c_info->WIN));

        if (left != right) {
            MPI_ERRHAND(MPI_Win_flush(left, c_info->WIN));

            MPI_ERRHAND(MPI_Win_flush(right, c_info->WIN));
        } else {
            MPI_ERRHAND(MPI_Win_flush(left, c_info->WIN));
        }
    }
    res_time = (MPI_Wtime() - res_time) / iterations->n_sample;
    if (left != right) {
        MPI_Win_unlock(left, c_info->WIN);
        MPI_Win_unlock(right, c_info->WIN);
    } else
        MPI_Win_unlock(left, c_info->WIN);

    /* Synchronize target and origin processes */
    MPI_Barrier(c_info->communicator);

#ifdef CHECK
    for (i = 0; i < ITER_MIN(iterations); i++) {
        CHK_DIFF("MPI_Put", c_info, (void*)(recv + size + i%iterations->r_cache_iter*iterations->r_offs),
                 size, size, size, asize, put, 0, iterations->n_sample, i, left, &defect);

        CHK_DIFF("MPI_Put", c_info, (void*)(recv + i%iterations->r_cache_iter*iterations->r_offs),
                 0, size, size, asize, put, 0, iterations->n_sample, i, right, &defect);
    }
#endif     

    *time = res_time;
    return;
}


static double IMB_put_target(struct iter_schedule* iterations, struct comm_info* c_info,
                             double time_to_compute) {
    double time = 0.;
    int i;
    time = MPI_Wtime();
    for (i = 0; i < iterations->n_sample; i++) {
        if (time_to_compute)
            IMB_cpu_exploit(time_to_compute, 0);

        MPI_Barrier(c_info->communicator);
    }
    time = (MPI_Wtime() - time) / iterations->n_sample;
    return time;
}

static double IMB_put_origin(int msg_size, int dst, struct comm_info* c_info,
                             struct iter_schedule* iterations)
{
    double time = 0.;
    int i;
    time = MPI_Wtime();
    for (i = 0; i < iterations->n_sample; i++) {
        MPI_Put((char*)c_info->s_buffer + i%iterations->s_cache_iter*iterations->s_offs,
                msg_size, c_info->s_data_type, dst, i%iterations->r_cache_iter*iterations->r_offs,
                msg_size, c_info->r_data_type, c_info->WIN);
        MPI_Win_flush(dst, c_info->WIN);
        MPI_Barrier(c_info->communicator);
    }
    time = (MPI_Wtime() - time) / iterations->n_sample;
    return time;
}

/* Implements "Truly_passive_put" benchmark. It allows to check whether
 * MPI implementation supports truly one sided mode.
 * The flow is:
 * 1) Execution time of MPI_Put() is measured for the case when the target is
 *    waiting in MPI_Barrier() call.
 * 2) This time is sent to the target
 * 3) Execution time of MPI_Put is measured for the case when the target performs
 *    some computation for about the time measued at step 1) and then enters
 *    MPI_Barrier() call */
void IMB_rma_passive_put(struct comm_info* c_info, int size,
                         struct iter_schedule* iterations,
                         MODES run_mode, double* time) {
    double t_pure = 0.;
    double t_with_comp = 0.;

    Type_Size s_size;
    int s_num = 0;

    time[0] = 0.;
    time[1] = 0.;
    if (c_info->rank < 0)
        return;

    /*  GET SIZE OF DATA TYPE */
    MPI_Type_size(c_info->s_data_type, &s_size);
    if (s_size != 0) {
        s_num = size / s_size;
    } else
        return;

    MPI_Barrier(c_info->communicator);

    if (c_info->rank == c_info->pair0) {
        MPI_Win_lock(MPI_LOCK_SHARED, c_info->pair1, 0, c_info->WIN);
        t_pure = IMB_put_origin(s_num, c_info->pair1, c_info, iterations);
        MPI_Send(&t_pure, 1, MPI_DOUBLE, c_info->pair1, 1, c_info->communicator);

        MPI_Barrier(c_info->communicator);

        t_with_comp = IMB_put_origin(s_num, c_info->pair1, c_info, iterations);
        time[0] = t_pure;
        time[1] = t_with_comp;

        MPI_Win_unlock(c_info->pair1, c_info->WIN);
    } else if (c_info->rank == c_info->pair1) {
        IMB_put_target(iterations, c_info, 0.);
        MPI_Recv(&t_pure, 1, MPI_DOUBLE, c_info->pair0, 1, c_info->communicator,
            MPI_STATUS_IGNORE);

        /*initialize IMB_cpu_exploit with received time*/
        IMB_cpu_exploit(t_pure, 1);

        MPI_Barrier(c_info->communicator);

        IMB_put_target(iterations, c_info, t_pure);
    }
    return;
}

