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
    
 File: IMB_bandwidth.c 

 Implemented functions: 

 IMB_uni_bandwidth; IMB_bi_bandwidth

***************************************************************************/

#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_prototypes.h"

/*************************************************************************/

void IMB_uni_bandwidth(struct comm_info* c_info, int size,  struct iter_schedule* ITERATIONS,
              MODES RUN_MODE, double* time) {
/*

                      MPI-1 benchmark kernel
                      multiple processes unidirectional exchange


Input variables:

-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information

-size                 (type int)
                      Basic message size in bytes

-ITERATIONS           (type struct iter_schedule *)
                      Repetition scheduling

-RUN_MODE             (type MODES)
                      (only MPI-2 case: see [1])


Output variables:

-time                 (type double*)
                      Timing result per sample


*/
    double t1,t2;
    int i;

    Type_Size s_size, r_size;
    int s_num = 0,
        r_num = 0;
    int s_tag, r_tag;
    int dest, source;
    MPI_Status stat;
    MPI_Request *requests = NULL;

    int ws, peers;
    char ack;

    MPI_Type_size(c_info->s_data_type, &s_size);
    MPI_Type_size(c_info->r_data_type, &r_size);
    if ((s_size != 0) && (r_size != 0)) {
        s_num = size / s_size;
        r_num = size / r_size;
    }
    else
        return;

    s_tag = 1;
    r_tag = s_tag;

    if (c_info->rank != -1)
        peers = c_info->num_procs / 2;
    else {
        *time = 0.;
        return;
    }

    requests = (MPI_Request*)malloc(c_info->max_win_size * sizeof(MPI_Request));
    for (i = 0; i < N_BARR; i++)
        MPI_Barrier(c_info->communicator);

    t1 = MPI_Wtime();
    if (c_info->rank < peers) {
        dest = (c_info->rank + peers);
        for(i = 0; i < ITERATIONS->n_sample; i++) {
            for (ws = 0; ws < c_info->max_win_size; ws++)
                MPI_ERRHAND(MPI_Isend((char*)c_info->s_buffer + ws % ITERATIONS->s_cache_iter * ITERATIONS->s_offs,
                                      s_num,
                                      c_info->s_data_type,
                                      dest,
                                      s_tag,
                                      c_info->communicator,
                                      &requests[ws]));

            MPI_Waitall(c_info->max_win_size, &requests[0], MPI_STATUSES_IGNORE);
            MPI_Recv(&ack, 1, MPI_CHAR, dest, r_tag, c_info->communicator, &stat);
        }
    }
    else {
        source = (c_info->rank - peers);
        for (i = 0; i < ITERATIONS->n_sample; i++) {
            for (ws = 0; ws < c_info->max_win_size; ws++)
                MPI_ERRHAND(MPI_Irecv((char*)c_info->r_buffer + ws % ITERATIONS->r_cache_iter * ITERATIONS->r_offs,
                                      r_num,
                                      c_info->r_data_type,
                                      source,
                                      r_tag,
                                      c_info->communicator,
                                      &requests[ws]));

            MPI_Waitall(c_info->max_win_size, &requests[0], MPI_STATUSES_IGNORE);
            MPI_Send(&ack, 1, MPI_CHAR, source, s_tag, c_info->communicator);
        }
    }
    t2 = MPI_Wtime();
    *time = (t2 - t1) / ITERATIONS->n_sample;

    free(requests);
}

void IMB_bi_bandwidth(struct comm_info* c_info, int size,  struct iter_schedule* ITERATIONS,
                      MODES RUN_MODE, double* time) {
/*

                      
                      MPI-1 benchmark kernel
                      multiple processes bidirectional exchange
                      


Input variables: 

-c_info               (type struct comm_info*)                      
                      Collection of all base data for MPI;
                      see [1] for more information
                      

-size                 (type int)                      
                      Basic message size in bytes

-ITERATIONS           (type struct iter_schedule *)
                      Repetition scheduling

-RUN_MODE             (type MODES)                      
                      (only MPI-2 case: see [1])


Output variables: 

-time                 (type double*)                      
                      Timing result per sample


*/
    double t1, t2;
    int i;

    Type_Size s_size, r_size;
    int s_num = 0,
        r_num = 0;
    int s_tag, r_tag;
    int dest, source;
    MPI_Status stat;
    const int max_win_size2 = 2 * c_info->max_win_size;
    MPI_Request *requests = NULL;

    int ws, peers;
    char ack;

    MPI_Type_size(c_info->s_data_type, &s_size);
    MPI_Type_size(c_info->r_data_type, &r_size);
    if ((s_size != 0) && (r_size != 0)) {
        s_num = size / s_size;
        r_num = size / r_size;
    }
    else
        return;

    s_tag = 1;
    r_tag = s_tag;

    if (c_info->rank!=-1)
        peers = c_info->num_procs / 2;
    else {
        *time = 0.;
        return;
    }

    requests = (MPI_Request*)malloc(2 * c_info->max_win_size * sizeof(MPI_Request));
    for (i = 0; i < N_BARR; i++)
        MPI_Barrier(c_info->communicator);

    t1 = MPI_Wtime();
    if (c_info->rank < peers) {
        dest = (c_info->rank + peers);
        for(i = 0; i < ITERATIONS->n_sample; i++) {
            for (ws = 0; ws < c_info->max_win_size; ws++)
                MPI_ERRHAND(MPI_Irecv((char*)c_info->r_buffer + ws % ITERATIONS->r_cache_iter * ITERATIONS->r_offs,
                                      r_num,
                                      c_info->r_data_type,
                                      dest,
                                      r_tag,
                                      c_info->communicator,
                                      &requests[ws]));

            for (ws = 0; ws < c_info->max_win_size; ws++)
                MPI_ERRHAND(MPI_Isend((char*)c_info->s_buffer + ws % ITERATIONS->s_cache_iter * ITERATIONS->s_offs,
                                      s_num,
                                      c_info->s_data_type,
                                      dest,
                                      s_tag,
                                      c_info->communicator,
                                      &requests[ws + c_info->max_win_size]));

            MPI_Waitall(max_win_size2, &requests[0], MPI_STATUSES_IGNORE);
            MPI_Recv(&ack, 1, MPI_CHAR, dest, r_tag, c_info->communicator, &stat);
        }
    }
    else {
        source = (c_info->rank - peers);
        for (i = 0; i < ITERATIONS->n_sample; i++) {
            for (ws = 0; ws < c_info->max_win_size; ws++)
                MPI_ERRHAND(MPI_Irecv((char*)c_info->r_buffer + ws % ITERATIONS->r_cache_iter * ITERATIONS->r_offs,
                                      r_num,
                                      c_info->r_data_type,
                                      source,
                                      r_tag,
                                      c_info->communicator,
                                      &requests[ws]));
            for (ws = 0; ws < c_info->max_win_size; ws++)
                MPI_ERRHAND(MPI_Isend((char*)c_info->s_buffer + ws % ITERATIONS->s_cache_iter * ITERATIONS->s_offs,
                                      s_num,
                                      c_info->s_data_type,
                                      source,
                                      s_tag,
                                      c_info->communicator,
                                      &requests[ws + c_info->max_win_size]));

            MPI_Waitall(max_win_size2, &requests[0], MPI_STATUSES_IGNORE);
            MPI_Send(&ack, 1, MPI_CHAR, source, s_tag, c_info->communicator);
        }
    }
    t2 = MPI_Wtime();
    *time = (t2 - t1) / ITERATIONS->n_sample;

    free(requests);
}
