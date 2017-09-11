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

typedef int assign_type ;
#define BUF_VALUE(rank,i)  10000000*(1+rank)+i%10000000

#endif

#ifdef BUFFERS_FLOAT

typedef float assign_type ;
#define BUF_VALUE(rank,i)  (0.1*((rank)+1)+(float)((i)))

#endif

#endif
