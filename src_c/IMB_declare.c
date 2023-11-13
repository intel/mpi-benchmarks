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

 File: IMB_declare.c 

 Implemented functions: 

 ***************************************************************************/





#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "IMB_settings.h"
#include "IMB_comm_info.h"
#include "IMB_err_check.h"


#ifdef CHECK

#include "IMB_prototypes.h"

long   r_check,s_check;
double defect;
double *all_defect;

#endif

FILE* dbg_file;
char* dbgf_name;

double *all_times;

/*  STRINGS FOR OUTPUT   */
char aux_string[out_fields*ow_format];

/* FORMAT FOR OUTPUT    */
char format [out_fields*7];

/* ARRAY OF CASES, EITHER DEFAULT OR ARGUMENT LIST */
char **cases ;

/* Error status  */

int err_flag;

/* I/O unit */
FILE* unit;

/* MAXIMAL MESSAGE LENGTH    */
int MAXMSG;

void * AUX;
size_t AUX_LEN=0;

int    do_nonblocking=0;
double tCPU = 0.;    /* CPU time for overlap benchmarks */
double MFlops = -1.;

/* calls to IMB_v_alloc / IMB_v_free */
int num_alloc=0, num_free=0;

#ifdef USE_MPI_INIT_THREAD
int mpi_thread_environment  = MPI_THREAD_SINGLE;
int mpi_thread_desired      = MPI_THREAD_MULTIPLE;
#endif /*USE_MPI_INIT_THREAD*/
