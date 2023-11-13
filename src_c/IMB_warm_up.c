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
    
 File: IMB_warm_up.c 

 Implemented functions: 

 IMB_warm_up;

 ***************************************************************************/





#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_prototypes.h"

/**********************************************************************/



/* IMB 3.1 << */
/*
Use ITERATIONS object;
perform warmup with the minimum message size, no longer with the maximum one;
*/
void IMB_warm_up(struct Bench* Bmark, struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS, int iter) {
/* >> IMB 3.1  */
/*

                      'Warm up' run of the particular benchmark, so the
                      system can eventually set up internal structures before
                      the actual benchmark

Input variables:

-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information

-Bmark                (type struct Bench*)
                      (For explanation of struct Bench type:
                      describes all aspects of modes of a benchmark;
                      see [1] for more information)

                      The actual benchmark

IMB 3.1 <<
-ITERATIONS           (type struct iter_schedule *)
                      Repetition scheduling
>> IMB 3.1

-iter                 (type int)
                      Number of the outer iteration of the benchmark. Only
                      for iter==0, the WamrUp is carried out

*/
#ifndef MPIIO

#ifndef RMA
    struct cmode MD;

    MD.AGGREGATE = 1;
#endif

    if (c_info->rank >= 0) {
        if (c_info->warm_up) {
            /* IMB 3.1: other warm up settings */
            double t[MAX_TIME_ID];
            int n_sample = ITERATIONS->n_sample;

            ITERATIONS->n_sample /= WARMUP_PERCENT;
            if ((ITERATIONS->n_sample == 0) && (n_sample > 1))
                ITERATIONS->n_sample = 1;
#ifdef MPI1
            c_info->select_source = Bmark->select_source;
#endif

#ifdef RMA
            Bmark->Benchmark(c_info, size, ITERATIONS, Bmark->RUN_MODES, t);

#else
            /* It is erroneous to pass unitialized MD to the bench. it may
             * depend on the particular mode values! Keep it for existing benchmarks
             * to save their bahvior */
            Bmark->Benchmark(c_info, size, ITERATIONS, &MD, t);
#endif

            ITERATIONS->n_sample = n_sample;
        }
    }
#endif
}

