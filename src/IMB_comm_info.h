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





#ifndef _COMM_INFO_H
#define _COMM_INFO_H 

#include "IMB_declare.h"


#ifdef MPIIO
typedef struct { int Locsize; MPI_Offset Offset; int Totalsize;} SPLITTING;
#endif

struct comm_info
{ 
/* Communication information as for MPI-1/2 parts */

    int 	w_num_procs;		/* number of procs in COMM_WORLD            */
    int 	w_rank;			/* rank of actual process in COMM_WORLD     */

    int 	NP;			/* #processes participating in benchmarks   */
    int 	px,py;			/* processes are part of px x py topology   */

    MPI_Comm 	communicator;		/* underlying communicator for benchmark(s) */

    int 	num_procs;		/* number of processes in communicator      */
    int 	rank;			/* rank of actual process in communicator   */
    int     root_shift;     /* switch for root change at each iteration */ 
    int     sync;           /* switch for rank synchronization after each iter */

    MPI_Datatype 	s_data_type;	/* data type of sent data                   */
    MPI_Datatype 	r_data_type;	/* data type of received data               */

    MPI_Datatype 	red_data_type;	/* data type of reduced data               */
    MPI_Op 		op_type;	/* operation type                          */

    int 	pair0, pair1;		/* process pair                            */
    int 	select_tag;		/* 0/1 for tag selection off/on            */
    int 	select_source;		/* 0/1 for sender selection off/on         */

    void* 		s_buffer;	/* send    buffer                          */
    assign_type*	s_data;		/* assign_type equivalent of s_buffer      */
    size_t		s_alloc;	/* #bytes allocated in s_buffer            */
    void* 		r_buffer;	/* receive buffer                          */
    assign_type* 	r_data;		/* assign_type equivalent of r_buffer      */
    size_t   		r_alloc;	/* #bytes allocated in r_buffer            */

/* IMB 3.1 << */
    float 	max_mem, used_mem;	/* max. allowed / used GBytes for all      */
					/* message  buffers                        */
/* >> IMB 3.1  */

    int 	n_lens;			/* # of selected lengths by -msglen option */
    int* 	msglen;			/* list of  "       "                  "   */

    int		group_mode;		/* Mode of running groups (<0,0,>0)        */
    int		n_groups;		/* No. of independent groups               */
    int		group_no;              	/* own group index                         */
    int*	g_sizes;		/* array of group sizes                    */
    int*	g_ranks;		/* w_ranks constituting the groups         */

    int* 	sndcnt;			/* send count argument for global ops.     */
    int*	sdispl;			/* displacement argument for global ops.   */
    int*	reccnt;			/* recv count argument for global ops.     */
    int*	rdispl;			/* displacement argument for global ops.   */

/* IMB 3.2.3 << */
    int		min_msg_log;
    int		max_msg_log;
/* >> IMB 3.2.3  */

    MPI_Errhandler	ERR;
 
#ifdef MPIIO
    /*   FILE INFORMATION     */
    char* 	filename;
    MPI_Comm 	File_comm;
    int		File_num_procs;
    int		all_io_procs;
    int		File_rank;

    MPI_File	fh;

    MPI_Datatype	etype;
    Type_Size		e_size;
    MPI_Datatype	filetype;

    SPLITTING	split;
    int 	amode;
    MPI_Info	info;
 
    /* View: */
    MPI_Offset	disp;
    char*	datarep;
    MPI_Datatype	view;
    MPI_Errhandler	ERRF;
#endif

#if (defined EXT || defined RMA)
    MPI_Win		WIN;
    MPI_Info		info;
    MPI_Errhandler	ERRW;
#endif

};

#endif
