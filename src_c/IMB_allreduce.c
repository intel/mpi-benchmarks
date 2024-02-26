/****************************************************************************
*                                                                           *
* Copyright (C) 2024 Intel Corporation                                      *
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
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE

 **********************************************************************

For more documentation than found here, see

[1] doc/ReadMe_IMB.txt 

[2] Intel(R) MPI Benchmarks
    Users Guide and Methodology Description
    In 
    doc/IMB_Users_Guide.pdf

 File: IMB_allreduce.c 

 Implemented functions: 

 IMB_allreduce;


 ***************************************************************************/





#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_prototypes.h"

#ifdef MPI1

/*******************************************************************************/


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

void IMB_allreduce(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                   MODES RUN_MODE, double* time) {
/*

                      MPI-1 benchmark kernel
                      Benchmarks MPI_Allreduce


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
    int    i;

    Type_Size s_size;
    int s_num = 0;
#ifdef CHECK
    int asize = (int) sizeof(assign_type);
    defect = 0.;
#endif

    *time = 0.;

    /*  GET SIZE OF DATA TYPE */
    MPI_Type_size(c_info->red_data_type, &s_size);
    if (s_size != 0)
        s_num = size / s_size;

    size *= c_info->size_scale;

    if (c_info->rank != -1) {
        IMB_do_n_barriers(c_info->communicator, N_BARR);

        for (i = 0; i < ITERATIONS->n_sample; i++) {
            *time -= MPI_Wtime();
            MPI_ERRHAND(MPI_Allreduce((char*)c_info->s_buffer + i % ITERATIONS->s_cache_iter * ITERATIONS->s_offs,
                                      (char*)c_info->r_buffer + i % ITERATIONS->r_cache_iter * ITERATIONS->r_offs,
                                      s_num,
                                      c_info->red_data_type,c_info->op_type,
                                      c_info->communicator));
            *time += MPI_Wtime();

            CHK_DIFF("Allreduce",c_info, (char*)c_info->r_buffer + i % ITERATIONS->r_cache_iter * ITERATIONS->r_offs, 0,
                     size, size, asize,
                     put, 0, ITERATIONS->n_sample, i,
                     -1, &defect);

            IMB_do_n_barriers (c_info->communicator, c_info->sync);

        }
        *time /= ITERATIONS->n_sample;
    }
}

#elif defined NBC // MPI1

/*************************************************************************/

void IMB_iallreduce(struct comm_info* c_info,
                    int size,
                    struct iter_schedule* ITERATIONS,
                    MODES RUN_MODE,
                    double* time) {
    int         i = 0;
    Type_Size   s_size;
    int         s_num = 0;
    MPI_Request request;
    MPI_Status  status;
    double      t_pure = 0.,
                t_comp = 0.,
                t_ovrlp = 0.;
#ifdef CHECK
    int asize = (int) sizeof(assign_type);
    defect = 0.;
#endif

    /* GET SIZE OF DATA TYPE */
    MPI_Type_size(c_info->red_data_type, &s_size);
    if (s_size != 0)
        s_num = size / s_size;

    if(c_info->rank != -1) {
        IMB_iallreduce_pure(c_info, size, ITERATIONS, RUN_MODE, &t_pure);

        /* INITIALIZATION CALL */
        IMB_cpu_exploit(t_pure, 1);

        IMB_do_n_barriers(c_info->communicator, N_BARR);

        for(i = 0; i < ITERATIONS->n_sample; i++)
        {
            t_ovrlp -= MPI_Wtime();
            MPI_ERRHAND(MPI_Iallreduce((char*)c_info->s_buffer + i % ITERATIONS->s_cache_iter * ITERATIONS->s_offs,
                                       (char*)c_info->r_buffer + i % ITERATIONS->r_cache_iter * ITERATIONS->r_offs,
                                       s_num,
                                       c_info->red_data_type,
                                       c_info->op_type,
                                       c_info->communicator,
                                       &request));
            
            t_comp -= MPI_Wtime();
            IMB_cpu_exploit(t_pure, 0);
            t_comp += MPI_Wtime();
            
            MPI_Wait(&request, &status);
            t_ovrlp += MPI_Wtime();
            
            CHK_DIFF("Iallreduce", c_info,
                     (char*)c_info->r_buffer + i % ITERATIONS->r_cache_iter * ITERATIONS->r_offs,
                     0, size, size, asize, put, 0, ITERATIONS->n_sample, i, -1, &defect);
            
            IMB_do_n_barriers(c_info->communicator, c_info->sync);
        }
        t_ovrlp /= ITERATIONS->n_sample;
        t_comp  /= ITERATIONS->n_sample;
    }

    time[0] = t_pure;
    time[1] = t_ovrlp;
    time[2] = t_comp;
}

/*************************************************************************/

void IMB_iallreduce_pure(struct comm_info* c_info,
                         int size,
                         struct iter_schedule* ITERATIONS,
                         MODES RUN_MODE,
                         double* time) {
    int         i = 0;
    Type_Size   s_size;
    int         s_num = 0;
    MPI_Request request;
    MPI_Status  status;
    double      t_pure = 0.;
#ifdef CHECK
    int asize = (int) sizeof(assign_type);
    defect = 0.;
#endif

    /* GET SIZE OF DATA TYPE */
    MPI_Type_size(c_info->red_data_type, &s_size);
    if (s_size != 0)
        s_num = size / s_size;

    if (c_info->rank != -1) {
        IMB_do_n_barriers(c_info->communicator, N_BARR);

        for (i = 0; i < ITERATIONS->n_sample; i++)
        {
            t_pure -= MPI_Wtime();
            MPI_ERRHAND(MPI_Iallreduce((char*)c_info->s_buffer + i % ITERATIONS->s_cache_iter * ITERATIONS->s_offs,
                                       (char*)c_info->r_buffer + i % ITERATIONS->r_cache_iter * ITERATIONS->r_offs,
                                       s_num,
                                       c_info->red_data_type,
                                       c_info->op_type,
                                       c_info->communicator,
                                       &request));
            MPI_Wait(&request, &status);
            t_pure += MPI_Wtime();

            CHK_DIFF("Iallreduce_pure", c_info,
                     (char*)c_info->r_buffer + i % ITERATIONS->r_cache_iter * ITERATIONS->r_offs,
                     0, size, size, asize, put, 0, ITERATIONS->n_sample, i, -1, &defect);
            IMB_do_n_barriers (c_info->communicator, c_info->sync);
        }
        t_pure /= ITERATIONS->n_sample;
    }

    time[0] = t_pure;
}

#endif // NBC // MPI1
