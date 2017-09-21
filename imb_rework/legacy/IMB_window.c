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
    
 File: IMB_window.c 

 Implemented functions: 

 IMB_window;

 ***************************************************************************/





#include "IMB_declare.h"
#include "IMB_benchmark.h"


#include "IMB_prototypes.h"


/*************************************************************************/



void IMB_window(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                MODES RUN_MODE, double* time)
/*

                      
                      MPI-2 benchmark kernel
                      MPI_Win_create + MPI_Win_fence + MPI_Win_free
                      


Input variables: 

-c_info               (type struct comm_info*)                      
                      Collection of all base data for MPI;
                      see [1] for more information
                      

-size                 (type int)                      
                      Basic message size in bytes

-ITERATIONS           (type struct iter_schedule)                      
                      Repetition scheduling

-RUN_MODE             (type MODES)                      
                      Mode (aggregate/non aggregate; blocking/nonblocking);
                      see "IMB_benchmark.h" for definition


Output variables: 

-time                 (type double*)                      
                      Timing result per sample


*/
{
  double t1, t2;
  int    i, dum;

  ierr = 0;

  if(c_info->rank!=-1)
    {
      for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->communicator);

      t1 = MPI_Wtime();
      for(i=0;i< ITERATIONS->n_sample;i++)
	{
          ierr = MPI_Win_create(c_info->r_buffer,size,1,MPI_INFO_NULL,
                                c_info->communicator, &c_info->WIN);
          MPI_ERRHAND(ierr);
          ierr = MPI_Win_fence(0, c_info->WIN);
          MPI_ERRHAND(ierr);
/* July 2002 fix V2.2.1, empty window case */
          if(size>0)
          {
          ierr = MPI_Put(c_info->s_buffer, 1, c_info->s_data_type,
                         c_info->rank, 0, 1, c_info->r_data_type, c_info->WIN);
          MPI_ERRHAND(ierr);
          }

          ierr = MPI_Win_fence(0, c_info->WIN);
          MPI_ERRHAND(ierr);
	  
          ierr = MPI_Win_free(&c_info->WIN);
          MPI_ERRHAND(ierr);
	}
      t2 = MPI_Wtime();
      *time=(t2 - t1)/(ITERATIONS->n_sample);
    }
  else
    { 
      *time = 0.; 
    }
}

