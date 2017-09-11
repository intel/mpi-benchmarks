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
int mpi_thread_environment = MPI_THREAD_SINGLE;
int mpi_thread_desired	   = MPI_THREAD_MULTIPLE;
#endif /*USE_MPI_INIT_THREAD*/
