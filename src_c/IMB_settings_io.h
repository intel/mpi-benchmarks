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





#ifndef _SETTINGS_H
#define _SETTINGS_H

/*
IMB settings definitions

There are two scenarios to run IMB

IMB_STANDARD:
"Mandatory" settings

IMB_OPTIONAL:
The user can change the settings, in order to extend the table of benchmark
results. Changeable are only a few crucial parameters for IMB.

Overall, at most 8 preprocessor varibales can bet set in order to control
IMB. These are

- IMB_OPTIONAL (has to be set when user optional settings are to be activated)

- MINMSGLOG    (second smallest data transfer size is 2^MINMSGLOG (the smallest
                always being 0))

- MAXMSGLOG    (largest data size is 2^MAXMSGLOG)

- MSGSPERSAMPLE (max. repetition count)
- MSGS_NONAGGR  (       "           for non aggregate benchmarks)

- OVERALL_VOL   (for all sizes < OVERALL_VOL, the repetition count is eventually
                reduced so that not more than OVERALL_VOL bytes overall are 
                processed.
                This avoids unnecessary repetitions for large message sizes.

                Finally, the real repetition count for message size X is

                MSGSPERSAMPLE (X=0),

                min(MSGPERSAMPLE,max(1,OVERALL_VOL/X))    (X>0)

                NOTE: OVERALL_VOL does NOT restrict the size of the max. 
                data transfer. 2^MAXMSGLOG is the largest size, independent
                of OVERALL_VOL.
               )


- N_BARR        Number of MPI_Barrier for synchronization

- TARGET_CPU_SECS
                CPU seconds (as float) to run concurrent with nonblocking
                benchmarks

In any case the user has to select the variable
FILENAME
which specifies the name of input/output file ("recycled" for all benchmarks;
if one separate file is opened on each process, an index _<rank> will be
appended to the filename)

*/ 

#define FILENAME "IMB_out"

#ifdef IMB_OPTIONAL
#else

/*
DON'T change anything below here !!
*/


#define MINMSGLOG 0
#define MAXMSGLOG 24

#define MSGSPERSAMPLE 50
#define MSGS_NONAGGR  10
#define OVERALL_VOL 16*1048576

#define SECS_PER_SAMPLE 10

#ifdef CHECK
#define TARGET_CPU_SECS 0.001
#else
#define TARGET_CPU_SECS 0.01
#endif

#define N_BARR   2


#endif

#define BUFFERS_INT

/* How to set communications buffers for process rank, index i */
#ifdef BUFFERS_INT

typedef int assign_type;
#define BUF_VALUE(rank,i)  10000000*(1+rank)+i%10000000

#endif

#ifdef BUFFERS_FLOAT

typedef float assign_type;
#define BUF_VALUE(rank,i)  (0.1*((rank)+1)+(float)((i)))

#endif

#endif
