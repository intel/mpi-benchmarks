/*****************************************************************************
 *                                                                           *
 * Copyright (c) 2003-2016 Intel Corporation.                                *
 * All rights reserved.                                                      *
 *                                                                           *
 *****************************************************************************

This code is covered by the Community Source License (CPL), version
1.0 as published by IBM and reproduced in the file "license.txt" in the
"license" subdirectory. Redistribution in source and binary form, with
or without modification, is permitted ONLY within the regulations
contained in above mentioned license.

Use of the name and trademark "Intel(R) MPI Benchmarks" is allowed ONLY
within the regulations of the "License for Use of "Intel(R) MPI
Benchmarks" Name and Trademark" as reproduced in the file
"use-of-trademark-license.txt" in the "license" subdirectory. 

THE PROGRAM IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT
LIMITATION, ANY WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT,
MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Each Recipient is
solely responsible for determining the appropriateness of using and
distributing the Program and assumes all risks associated with its
exercise of rights under this Agreement, including but not limited to
the risks and costs of program errors, compliance with applicable
laws, damage to or loss of data, programs or equipment, and
unavailability or interruption of operations.

EXCEPT AS EXPRESSLY SET FORTH IN THIS AGREEMENT, NEITHER RECIPIENT NOR
ANY CONTRIBUTORS SHALL HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING
WITHOUT LIMITATION LOST PROFITS), HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OR
DISTRIBUTION OF THE PROGRAM OR THE EXERCISE OF ANY RIGHTS GRANTED
HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 

EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF
YOUR JURISDICTION. It is licensee's responsibility to comply with any
export regulations applicable in licensee's jurisdiction. Under
CURRENT U.S. export regulations this software is eligible for export
from the U.S. and can be downloaded by or otherwise exported or
reexported worldwide EXCEPT to U.S. embargoed destinations which
include Cuba, Iraq, Libya, North Korea, Iran, Syria, Sudan,
Afghanistan and any other country to which the U.S. has embargoed
goods and services.

 ***************************************************************************

For more documentation than found here, see

[1] doc/ReadMe_IMB.txt 

[2] Intel (R) MPI Benchmarks
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
void IMB_warm_up (struct comm_info* c_info, struct Bench* Bmark, struct iter_schedule* ITERATIONS, int iter)
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
{
    struct cmode MD;

    MD.AGGREGATE = 1;

    if( c_info->rank >= 0 )
    {
#ifndef MPIIO
        if( iter == 0 )
        {
/* IMB 3.1: other warm up settings */
            int size = asize;
            double t[MAX_TIME_ID];
            int n_sample = ITERATIONS->n_sample;

            ITERATIONS->n_sample = N_WARMUP;
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
#endif
    }
}

