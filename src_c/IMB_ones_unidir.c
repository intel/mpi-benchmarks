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
    
 File: IMB_ones_unidir.c 

 Implemented functions: 

 IMB_unidir_put;
 IMB_unidir_get;
 IMB_ones_get;
 IMB_ones_mget;
 IMB_ones_put;
 IMB_ones_mput;

 ***************************************************************************/





#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_prototypes.h"

/*************************************************/


/* ===================================================================== */
/* 
IMB 3.1 changes
July 2007
Hans-Joachim Plum, Intel GmbH

- replace "int n_sample" by iteration scheduling object "ITERATIONS"
  (see => IMB_benchmark.h)

- proceed with offsets in send / recv buffers to eventually provide
  out-of-cache data
*/
/* ===================================================================== */


void IMB_unidir_put(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                    MODES RUN_MODE, double* time) {
/*

                          MPI-2 benchmark kernel
                          Driver for aggregate / non agg. unidirectional MPI_Put benchmarks

Input variables:

-c_info                   (type struct comm_info*)
                          Collection of all base data for MPI;
                          see [1] for more information

-size                     (type int)
                          Basic message size in bytes

-ITERATIONS               (type struct iter_schedule *)
                          Repetition scheduling

-RUN_MODE                 (type MODES)
                          Mode (aggregate/non aggregate; blocking/nonblocking);
                          see "IMB_benchmark.h" for definition

Output variables:

-time                     (type double*)
                          Timing result per sample

*/

    Type_Size s_size, r_size;
    int s_num = 0,
        r_num = 0;
    int dest, sender;

    /*  GET SIZE OF DATA TYPE */
    MPI_Type_size(c_info->s_data_type, &s_size);
    MPI_Type_size(c_info->r_data_type, &r_size);
    if ((s_size != 0) && (r_size != 0)) {
        s_num = size / s_size;
        r_num = size / r_size;
    }

    if (c_info->rank == c_info->pair0) {
        dest = c_info->pair1;
        sender = 1;
    } else if (c_info->rank == c_info->pair1) {
        dest = c_info->pair0;
        sender = 0;
    } else {
        dest = -1;
        sender = -1;
    }

    if (!RUN_MODE->AGGREGATE)
        IMB_ones_put(c_info,
                     s_num, dest,
                     r_num, sender,
                     size, ITERATIONS,
                     time);
    if (RUN_MODE->AGGREGATE)
        IMB_ones_mput(c_info,
                      s_num, dest,
                      r_num, sender,
                      size, ITERATIONS,
                      time);
}

/*************************************************************************/



void IMB_unidir_get(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                    MODES RUN_MODE, double* time) {
/*

                          MPI-2 benchmark kernel
                          Driver for aggregate / non agg. unidirectional MPI_Get benchmarks

Input variables:

-c_info                   (type struct comm_info*)
                          Collection of all base data for MPI;
                          see [1] for more information

-size                     (type int)
                          Basic message size in bytes

-ITERATIONS               (type struct iter_schedule *)
                          Repetition scheduling

-RUN_MODE                 (type MODES)
                          Mode (aggregate/non aggregate; blocking/nonblocking);
                          see "IMB_benchmark.h" for definition

Output variables:

-time                     (type double*)
                          Timing result per sample

*/

    Type_Size s_size, r_size;
    int s_num = 0,
        r_num = 0;
    int dest, sender;

    /*  GET SIZE OF DATA TYPE */
    MPI_Type_size(c_info->s_data_type, &s_size);
    MPI_Type_size(c_info->r_data_type, &r_size);
    if ((s_size != 0) && (r_size != 0)) {
        s_num = size / s_size;
        r_num = size / r_size;
    }

    if (c_info->rank == c_info->pair0) {
        dest = c_info->pair1;
        sender = 1;
    } else if (c_info->rank == c_info->pair1) {
        dest = c_info->pair0;
        sender = 0;
    } else {
        dest = -1;
        sender = -1;
    }


    if (!RUN_MODE->AGGREGATE)
        IMB_ones_get(c_info,
                     s_num, dest,
                     r_num, sender,
                     size, ITERATIONS,
                     time);
    if (RUN_MODE->AGGREGATE)
        IMB_ones_mget(c_info,
                      s_num, dest,
                      r_num, sender,
                      size, ITERATIONS,
                      time);
}




void IMB_ones_get(struct comm_info* c_info, int s_num, int dest,
                  int r_num, int sender, int size,
                  struct iter_schedule *ITERATIONS, double* time) {
/*

                          Non aggregate MPI_Get + MPI_Win_fence

Input variables:

-c_info                   (type struct comm_info*)
                          Collection of all base data for MPI;
                          see [1] for more information

-s_num                    (type int)
                          #buffer entries to put if relevant for calling process

-dest                     (type int)
                          destination rank

-r_num                    (type int)
                          #buffer entries to get if relevant for calling process

-sender                   (type int)
                          logical flag: 1/0 for 'local process puts/gets'

-size                     (type int)
                          Basic message size in bytes

-ITERATIONS               (type struct iter_schedule *)
                          Repetition scheduling

-RUN_MODE                 (type MODES)
                          Mode (aggregate/non aggregate; blocking/nonblocking);
                          see "IMB_benchmark.h" for definition

Output variables:

-time                     (type double*)
                          Timing result per sample

*/
    int i;
    int s_size;
#ifdef CHECK 
    int asize = (int) sizeof(assign_type);
    defect = 0;
#endif

    MPI_Type_size(c_info->s_data_type, &s_size);

    if (c_info->rank < 0)
        *time = 0.;
    else {
        for (i = 0; i < N_BARR; i++)
            MPI_Barrier(c_info->communicator);

        *time = MPI_Wtime();

        if (sender) {
            for (i = 0; i < ITERATIONS->n_sample; i++) {
                /* "Send ", i.e. synchronize window */
                MPI_ERRHAND(MPI_Win_fence(0, c_info->WIN));
            }
        } else {
            for (i = 0; i < ITERATIONS->n_sample; i++) {
                /* "Receive" */
                MPI_ERRHAND(MPI_Get((char*)c_info->r_buffer + i%ITERATIONS->r_cache_iter*ITERATIONS->r_offs,
                                    r_num, c_info->r_data_type,
                                    dest, i%ITERATIONS->s_cache_iter*ITERATIONS->s_offs,
                                    s_num, c_info->s_data_type, c_info->WIN));
                MPI_ERRHAND(MPI_Win_fence(0, c_info->WIN));

                DIAGNOSTICS("MPI_Get: ", c_info, c_info->r_buffer, r_num, r_num, i, 0);

                CHK_DIFF("MPI_Get", c_info, (void*)((char*)c_info->r_buffer + i%ITERATIONS->r_cache_iter*ITERATIONS->r_offs), 0,
                         size, size, asize,
                         get, 0, ITERATIONS->n_sample, i,
                         dest, &defect);
            }
        }
        *time = (MPI_Wtime() - *time) / ITERATIONS->n_sample;
    }
}

void IMB_ones_mget(struct comm_info* c_info, int s_num, int dest,
                   int r_num, int sender, int size,
                   struct iter_schedule* ITERATIONS, double* time) {
/*

                          Aggregate MPI_Get + MPI_Win_fence

Input variables:

-c_info                   (type struct comm_info*)
                          Collection of all base data for MPI;
                          see [1] for more information

-s_num                    (type int)
                          #buffer entries to put if relevant for calling process

-dest                     (type int)
                          destination rank

-r_num                    (type int)
                          #buffer entries to get if relevant for calling process

-sender                   (type int)
                          logical flag: 1/0 for 'local process puts/gets'

-size                     (type int)
                          Basic message size in bytes

-ITERATIONS               (type struct iter_schedule *)
                          Repetition scheduling

-RUN_MODE                 (type MODES)
                          Mode (aggregate/non aggregate; blocking/nonblocking);
                          see "IMB_benchmark.h" for definition

Output variables:

-time                     (type double*)
                          Timing result per sample

*/
    int i;
    char* recv;

#ifdef CHECK 
    int asize = (int) sizeof(assign_type);
    defect = 0;
#endif

    if (c_info->rank < 0)
        *time = 0.;
    else {
        recv = (char*)c_info->r_buffer;

        MPI_ERRHAND(MPI_Win_fence(0, c_info->WIN));

        for (i = 0; i < N_BARR; i++)
            MPI_Barrier(c_info->communicator);

        *time = MPI_Wtime();

        if (!sender)
            for (i = 0; i < ITERATIONS->n_sample; i++) {
                MPI_ERRHAND(MPI_Get((void*)(recv + i%ITERATIONS->r_cache_iter*ITERATIONS->r_offs),
                                    r_num, c_info->r_data_type,
                                    dest, i%ITERATIONS->s_cache_iter*ITERATIONS->s_offs,
                                    s_num, c_info->s_data_type, c_info->WIN));
            }
        MPI_ERRHAND(MPI_Win_fence(0, c_info->WIN));

        *time = (MPI_Wtime() - *time) / ITERATIONS->n_sample;

#ifdef CHECK
        if (!sender) {
            for (i = 0; i < ITERATIONS->n_sample; i++) {
                CHK_DIFF("MPI_Get", c_info, (void*)((char*)c_info->r_buffer + i%ITERATIONS->r_cache_iter*ITERATIONS->r_offs), 0,
                         size, size, asize, 
                         get, 0, ITERATIONS->n_sample, i,
                         dest, &defect);
            }
        }
#endif
    }
}

void IMB_ones_put(struct comm_info* c_info, int s_num, int dest,
                  int r_num, int sender, int size,
                  struct iter_schedule* ITERATIONS, double* time) {
/*

                          Non aggregate MPI_Put + MPI_Win_fence

Input variables:

-c_info                   (type struct comm_info*)
                          Collection of all base data for MPI;
                          see [1] for more information

-s_num                    (type int)
                          #buffer entries to put if relevant for calling process

-dest                     (type int)
                          destination rank

-r_num                    (type int)
                          #buffer entries to get if relevant for calling process

-sender                   (type int)
                          logical flag: 1/0 for 'local process puts/gets'

-size                     (type int)
                          Basic message size in bytes

-ITERATIONS               (type struct iter_schedule *)
                          Repetition scheduling

-RUN_MODE                 (type MODES)
                          Mode (aggregate/non aggregate; blocking/nonblocking);
                          see "IMB_benchmark.h" for definition

Output variables:

-time                     (type double*)
                          Timing result per sample

*/
    int i, r_size;

#ifdef CHECK 
    int asize = (int) sizeof(assign_type);
    defect = 0;
#endif

    MPI_Type_size(c_info->r_data_type, &r_size);

    if (c_info->rank < 0)
        *time = 0.;
    else {
        for (i = 0; i < N_BARR; i++)
            MPI_Barrier(c_info->communicator);

        *time = MPI_Wtime();

        if (sender) {
            for (i = 0; i < ITERATIONS->n_sample; i++) {
                /* Send */
                MPI_ERRHAND(MPI_Put((char*)c_info->s_buffer + i%ITERATIONS->s_cache_iter*ITERATIONS->s_offs,
                                    s_num, c_info->s_data_type,
                                    dest, i%ITERATIONS->r_cache_iter*ITERATIONS->r_offs,
                                    r_num, c_info->r_data_type, c_info->WIN));
                MPI_ERRHAND(MPI_Win_fence(0, c_info->WIN));
            }
        } else {
            for (i = 0; i < ITERATIONS->n_sample; i++) {
                /* "Receive", i.e. synchronize the window */
                MPI_ERRHAND(MPI_Win_fence(0, c_info->WIN));

                DIAGNOSTICS("MPI_Put: ", c_info, c_info->r_buffer, r_num, r_num, i, 0);

                CHK_DIFF("MPI_Put", c_info, (void*)((char*)c_info->r_buffer + i%ITERATIONS->r_cache_iter*ITERATIONS->r_offs), 0,
                    size, size, asize,
                    get, 0, ITERATIONS->n_sample, i,
                    dest, &defect);
            }
        }
        *time = (MPI_Wtime() - *time) / ITERATIONS->n_sample;
    }
}




void IMB_ones_mput(struct comm_info* c_info, int s_num, int dest,
                   int r_num, int sender, int size,
                   struct iter_schedule* ITERATIONS, double* time) {
/*

                          Aggregate MPI_Put + MPI_Win_fence

Input variables:

-c_info                   (type struct comm_info*)
                          Collection of all base data for MPI;
                          see [1] for more information

-s_num                    (type int)
                          #buffer entries to put if relevant for calling process

-dest                     (type int)
                          destination rank

-r_num                    (type int)
                          #buffer entries to get if relevant for calling process

-sender                   (type int)
                          logical flag: 1/0 for 'local process puts/gets'

-size                     (type int)
                          Basic message size in bytes

-ITERATIONS               (type struct iter_schedule *)
                          Repetition scheduling

-RUN_MODE                 (type MODES)
                          Mode (aggregate/non aggregate; blocking/nonblocking);
                          see "IMB_benchmark.h" for definition

Output variables:

-time                     (type double*)
                          Timing result per sample

*/
    int i;
    char* send;

#ifdef CHECK
    int asize = (int) sizeof(assign_type);
    defect = 0;
#endif

    if (c_info->rank < 0)
        *time = 0.;
    else {
        send = (char*)c_info->s_buffer;

        MPI_ERRHAND(MPI_Win_fence(0, c_info->WIN));

        for (i = 0; i < N_BARR; i++)
            MPI_Barrier(c_info->communicator);

        *time = MPI_Wtime();

        if (sender)
            for (i = 0; i < ITERATIONS->n_sample; i++) {
                MPI_ERRHAND(MPI_Put((void*)(send + i%ITERATIONS->s_cache_iter*ITERATIONS->s_offs),
                                    s_num, c_info->s_data_type,
                                    dest, i%ITERATIONS->r_cache_iter*ITERATIONS->r_offs,
                                    r_num, c_info->r_data_type, c_info->WIN));
            }

        MPI_ERRHAND(MPI_Win_fence(0, c_info->WIN));

        *time = (MPI_Wtime() - *time) / ITERATIONS->n_sample;

        if (!sender)
            for (i = 0; i < ITERATIONS->n_sample; i++) {
                CHK_DIFF("MPI_Put", c_info, (void*)((char*)c_info->r_buffer + i%ITERATIONS->r_cache_iter*ITERATIONS->r_offs), 0,
                         size, size, asize,
                         get, 0, ITERATIONS->n_sample, i,
                         dest, &defect);
            }
    }
}
