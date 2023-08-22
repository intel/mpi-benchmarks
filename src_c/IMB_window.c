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
    
 File: IMB_window.c 

 Implemented functions: 

 IMB_window;

 ***************************************************************************/





#include "IMB_declare.h"
#include "IMB_benchmark.h"


#include "IMB_prototypes.h"


/*************************************************************************/



void IMB_window(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                MODES RUN_MODE, double* time) {
/*

                          MPI-2 benchmark kernel
                          MPI_Win_create + MPI_Win_fence + MPI_Win_free

Input variables:

-c_info                   (type struct comm_info*)
                          Collection of all base data for MPI;
                          see [1] for more information

-size                     (type int)
                          Basic message size in bytes

-ITERATIONS               (type struct iter_schedule)
                          Repetition scheduling

-RUN_MODE                 (type MODES)
                          Mode (aggregate/non aggregate; blocking/nonblocking);
                          see "IMB_benchmark.h" for definition

Output variables:

-time                     (type double*)
                          Timing result per sample

*/
    double t1, t2;
    int    i;

    if (c_info->rank != -1) {
        for (i = 0; i < N_BARR; i++)
            MPI_Barrier(c_info->communicator);

        t1 = MPI_Wtime();
        for (i = 0; i < ITERATIONS->n_sample; i++) {
            MPI_ERRHAND(MPI_Win_create(c_info->r_buffer, size, 1, MPI_INFO_NULL,
                                       c_info->communicator, &c_info->WIN));
            MPI_ERRHAND(MPI_Win_fence(0, c_info->WIN));
            /* July 2002 fix V2.2.1, empty window case */
            if (size > 0) {
                MPI_ERRHAND(MPI_Put(c_info->s_buffer, 1, c_info->s_data_type,
                                    c_info->rank, 0, 1, c_info->r_data_type, c_info->WIN));
            }

            MPI_ERRHAND(MPI_Win_fence(0, c_info->WIN));

            MPI_ERRHAND(MPI_Win_free(&c_info->WIN));
        }
        t2 = MPI_Wtime();
        *time = (t2 - t1) / (ITERATIONS->n_sample);
    }
    else
        *time = 0.;
}

