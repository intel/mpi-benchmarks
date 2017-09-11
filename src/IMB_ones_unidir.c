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
    
 File: IMB_ones_unidir.c 

 Implemented functions: 

 IMB_unidir_put;
 IMB_unidir_get;
 IMB_ones_get;
 IMB_ones_mget;
 IMB_ones_put;
 IMB_ones_mput;

 ***************************************************************************/





#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_prototypes.h"

/*************************************************/


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


void IMB_unidir_put (struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS, 
                     MODES RUN_MODE, double* time)
/*

                      
                      MPI-2 benchmark kernel
                      Driver for aggregate / non agg. unidirectional MPI_Put benchmarks
                      


Input variables: 

-c_info               (type struct comm_info*)                      
                      Collection of all base data for MPI;
                      see [1] for more information
                      

-size                 (type int)                      
                      Basic message size in bytes

-ITERATIONS           (type struct iter_schedule *)                      
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
  
  Type_Size s_size,r_size;
  int s_num, r_num;
  int dest, source,sender;
  MPI_Status stat;

  ierr = 0;
  /*  GET SIZE OF DATA TYPE */  
  MPI_Type_size(c_info->s_data_type,&s_size);
  MPI_Type_size(c_info->r_data_type,&r_size);
  if ((s_size!=0) && (r_size!=0))
    {
      s_num=size/s_size;
      r_num=size/r_size;
    } 

  if (c_info->rank == c_info->pair0)
    {
      dest = c_info->pair1;
      sender = 1;
    }
  else if (c_info->rank == c_info->pair1)
    {
      dest =c_info->pair0 ;
      sender = 0;
    } else
    {
      dest   = -1;
      sender = -1;
    }
 
  if( !RUN_MODE->AGGREGATE )
     IMB_ones_put(  c_info,
                s_num, dest, 
                r_num, sender,
                size, ITERATIONS,
                time);
  if( RUN_MODE->AGGREGATE )
     IMB_ones_mput( c_info,
                s_num, dest, 
                r_num, sender,
                size, ITERATIONS,
                time);

}

/*************************************************************************/



void IMB_unidir_get (struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                     MODES RUN_MODE, double* time)
/*

                      
                      MPI-2 benchmark kernel
                      Driver for aggregate / non agg. unidirectional MPI_Get benchmarks
                      


Input variables: 

-c_info               (type struct comm_info*)                      
                      Collection of all base data for MPI;
                      see [1] for more information
                      

-size                 (type int)                      
                      Basic message size in bytes

-ITERATIONS           (type struct iter_schedule *)
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
  
  Type_Size s_size,r_size;
  int s_num, r_num;
  int dest, source,sender;
  int ierr;
  MPI_Status stat;

  /*  GET SIZE OF DATA TYPE */  
  MPI_Type_size(c_info->s_data_type,&s_size);
  MPI_Type_size(c_info->r_data_type,&r_size);
  if ((s_size!=0) && (r_size!=0))
    {
      s_num=size/s_size;
      r_num=size/r_size;
    } 

  if (c_info->rank == c_info->pair0)
    {
      dest = c_info->pair1;
      sender = 1;
    }
  else if (c_info->rank == c_info->pair1)
    {
      dest =c_info->pair0 ;
      sender = 0;
    }
  else
  {
      dest   = -1;
      sender = -1;
  }
 

  if( !RUN_MODE->AGGREGATE )
     IMB_ones_get(  c_info,
                s_num, dest, 
                r_num, sender,
                size, ITERATIONS,
                time);
  if( RUN_MODE->AGGREGATE )
     IMB_ones_mget( c_info,
                s_num, dest, 
                r_num, sender,
                size, ITERATIONS,
                time);
  
}




void IMB_ones_get(struct comm_info* c_info, int s_num, int dest, 
                  int r_num, int sender, int size, 
                  struct iter_schedule *ITERATIONS, double* time)
/*

                      
                      Non aggregate MPI_Get + MPI_Win_fence
                      


Input variables: 

-c_info               (type struct comm_info*)                      
                      Collection of all base data for MPI;
                      see [1] for more information
                      

-s_num                (type int)                      
                      #buffer entries to put if relevant for calling process 
                      

-dest                 (type int)                      
                      destination rank
                      

-r_num                (type int)                      
                      #buffer entries to get if relevant for calling process 
                      

-sender               (type int)                      
                      logical flag: 1/0 for 'local process puts/gets'
                      

-size                 (type int)                      
                      Basic message size in bytes
                      

-ITERATIONS           (type struct iter_schedule *)                      
                      Repetition scheduling


-RUN_MODE             (type MODES)                      
                      Mode (aggregate/non aggregate; blocking/nonblocking);
                      see "IMB_benchmark.h" for definition
                      


Output variables: 

-time                 (type double*)                      
                      Timing result per sample
                      


*/
{
int i, ierr;
int s_size;

#ifdef CHECK 
  defect=0;
#endif

MPI_Type_size(c_info->s_data_type,&s_size);

if( c_info-> rank < 0 )
*time = 0.;
else
{

for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->communicator);

*time = MPI_Wtime();

if( sender ) 
{

for(i=0;i< ITERATIONS->n_sample;i++)
	{

/* "Send ", i.e. synchronize window */

       ierr = MPI_Win_fence(0, c_info->WIN);
       MPI_ERRHAND(ierr);

	}
}
else
{

for(i=0;i< ITERATIONS->n_sample;i++)
	{

/* "Receive" */
       ierr = MPI_Get((char*)c_info->r_buffer+i%ITERATIONS->r_cache_iter*ITERATIONS->r_offs,
                      r_num, c_info->r_data_type,
                      dest, i%ITERATIONS->s_cache_iter*ITERATIONS->s_offs,
                      s_num, c_info->s_data_type, c_info->WIN);
       ierr = MPI_Win_fence(0, c_info->WIN);

       MPI_ERRHAND(ierr);

       DIAGNOSTICS("MPI_Get: ",c_info,c_info->r_buffer,r_num,r_num,i,0);

       CHK_DIFF("MPI_Get",c_info, (void*)((char*)c_info->r_buffer+i%ITERATIONS->r_cache_iter*ITERATIONS->r_offs),0,
                 size, size, asize,
                 get, 0, ITERATIONS->n_sample, i,
                 dest, &defect);
	}

}

*time=(MPI_Wtime()-*time)/ITERATIONS->n_sample;

}
}




void IMB_ones_mget(struct comm_info* c_info, int s_num, int dest, 
                   int r_num, int sender, int size, 
                   struct iter_schedule* ITERATIONS, double* time)
/*

                      
                      Aggregate MPI_Get + MPI_Win_fence
                      


Input variables: 

-c_info               (type struct comm_info*)                      
                      Collection of all base data for MPI;
                      see [1] for more information
                      

-s_num                (type int)                      
                      #buffer entries to put if relevant for calling process 
                      

-dest                 (type int)                      
                      destination rank
                      

-r_num                (type int)                      
                      #buffer entries to get if relevant for calling process 
                      

-sender               (type int)                      
                      logical flag: 1/0 for 'local process puts/gets'
                      

-size                 (type int)                      
                      Basic message size in bytes
                      

-ITERATIONS           (type struct iter_schedule *)                      
                      Repetition scheduling


-RUN_MODE             (type MODES)                      
                      Mode (aggregate/non aggregate; blocking/nonblocking);
                      see "IMB_benchmark.h" for definition
                      


Output variables: 

-time                 (type double*)                      
                      Timing result per sample
                      


*/
{
int i, ierr;
char* recv;

#ifdef CHECK 
defect=0;
#endif

if( c_info-> rank < 0 )
*time = 0.;
else
{
recv = (char*)c_info->r_buffer;

ierr = MPI_Win_fence(0, c_info->WIN);
MPI_ERRHAND(ierr);

for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->communicator);

*time = MPI_Wtime();

if( !sender )
for(i=0;i< ITERATIONS->n_sample;i++)
	{
        ierr = MPI_Get((void*)(recv+i%ITERATIONS->r_cache_iter*ITERATIONS->r_offs), 
                       r_num, c_info->r_data_type,
                       dest, i%ITERATIONS->s_cache_iter*ITERATIONS->s_offs,
                       s_num, c_info->s_data_type, c_info->WIN);
	}

ierr = MPI_Win_fence(0, c_info->WIN);
MPI_ERRHAND(ierr);

*time=(MPI_Wtime()-*time)/ITERATIONS->n_sample;

#ifdef CHECK
if(!sender)
{
for(i=0;i< ITERATIONS->n_sample;i++)
	{
       CHK_DIFF("MPI_Get",c_info, (void*)((char*)c_info->r_buffer+i%ITERATIONS->r_cache_iter*ITERATIONS->r_offs),0,
                 size, size, asize,
                 get, 0, ITERATIONS->n_sample, i,
                 dest, &defect);
        }
}
#endif

}
}




void IMB_ones_put(struct comm_info* c_info, int s_num, int dest, 
                  int r_num, int sender, int size, 
                  struct iter_schedule* ITERATIONS,  double* time)
/*

                      
                      Non aggregate MPI_Put + MPI_Win_fence
                      


Input variables: 

-c_info               (type struct comm_info*)                      
                      Collection of all base data for MPI;
                      see [1] for more information
                      

-s_num                (type int)                      
                      #buffer entries to put if relevant for calling process 
                      

-dest                 (type int)                      
                      destination rank
                      

-r_num                (type int)                      
                      #buffer entries to get if relevant for calling process 
                      

-sender               (type int)                      
                      logical flag: 1/0 for 'local process puts/gets'
                      

-size                 (type int)                      
                      Basic message size in bytes
                      

-ITERATIONS           (type struct iter_schedule *)                      
                      Repetition scheduling


-RUN_MODE             (type MODES)                      
                      Mode (aggregate/non aggregate; blocking/nonblocking);
                      see "IMB_benchmark.h" for definition
                      


Output variables: 

-time                 (type double*)                      
                      Timing result per sample
                      


*/
{
int i, ierr, r_size;
char* recv;

#ifdef CHECK 
defect=0;
#endif

MPI_Type_size(c_info->r_data_type,&r_size);

recv = (char*)c_info->r_buffer;

if( c_info-> rank < 0 )
*time = 0.;
else
{

for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->communicator);

*time = MPI_Wtime();

if( sender ) 
{

for(i=0;i< ITERATIONS->n_sample;i++)
	{

/* Send */

       ierr = MPI_Put((char*)c_info->s_buffer+i%ITERATIONS->s_cache_iter*ITERATIONS->s_offs,
                      s_num, c_info->s_data_type,
                      dest, i%ITERATIONS->r_cache_iter*ITERATIONS->r_offs,
                      r_num, c_info->r_data_type, c_info->WIN);

       ierr = MPI_Win_fence(0, c_info->WIN);
       MPI_ERRHAND(ierr);

        }
}
else
{

for(i=0;i< ITERATIONS->n_sample;i++)
	{

/* "Receive", i.e. synchronize the window */
       ierr = MPI_Win_fence(0, c_info->WIN);
       MPI_ERRHAND(ierr);

       DIAGNOSTICS("MPI_Put: ",c_info,c_info->r_buffer,r_num,r_num,i,0);

       CHK_DIFF("MPI_Put",c_info, (void*)(recv+i%ITERATIONS->r_cache_iter*ITERATIONS->r_offs), 0,
                 size, size, asize,
                 get, 0, ITERATIONS->n_sample, i,
                 dest, &defect);

        }
}

*time=(MPI_Wtime()-*time)/ITERATIONS->n_sample;

}
}




void IMB_ones_mput(struct comm_info* c_info, int s_num, int dest, 
                   int r_num, int sender, int size, 
                   struct iter_schedule* ITERATIONS, double* time)
/*

                      
                      Aggregate MPI_Put + MPI_Win_fence
                      


Input variables: 

-c_info               (type struct comm_info*)                      
                      Collection of all base data for MPI;
                      see [1] for more information
                      

-s_num                (type int)                      
                      #buffer entries to put if relevant for calling process 
                      

-dest                 (type int)                      
                      destination rank
                      

-r_num                (type int)                      
                      #buffer entries to get if relevant for calling process 
                      

-sender               (type int)                      
                      logical flag: 1/0 for 'local process puts/gets'
                      

-size                 (type int)                      
                      Basic message size in bytes
                      

-ITERATIONS           (type struct iter_schedule *)                      
                      Repetition scheduling


-RUN_MODE             (type MODES)                      
                      Mode (aggregate/non aggregate; blocking/nonblocking);
                      see "IMB_benchmark.h" for definition
                      


Output variables: 

-time                 (type double*)                      
                      Timing result per sample
                      


*/
{
int i, ierr;
char* send, *recv;

#ifdef CHECK 
defect=0;
#endif

if( c_info-> rank < 0 )
*time = 0.;
else
{
send = (char*)c_info->s_buffer;
recv = (char*)c_info->r_buffer;

ierr = MPI_Win_fence(0, c_info->WIN);
MPI_ERRHAND(ierr);

for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->communicator);

*time = MPI_Wtime();

if( sender )
for(i=0;i< ITERATIONS->n_sample;i++)
	{
          ierr = MPI_Put((void*)(send+i%ITERATIONS->s_cache_iter*ITERATIONS->s_offs),
                         s_num, c_info->s_data_type,
                         dest, i%ITERATIONS->r_cache_iter*ITERATIONS->r_offs,
                         r_num, c_info->r_data_type, c_info->WIN);
          MPI_ERRHAND(ierr);
	}

ierr = MPI_Win_fence(0, c_info->WIN);
MPI_ERRHAND(ierr);

*time=(MPI_Wtime()-*time)/ITERATIONS->n_sample;

if(!sender)
for(i=0;i< ITERATIONS->n_sample;i++)
	{
       CHK_DIFF("MPI_Put",c_info, (void*)(recv+i%ITERATIONS->r_cache_iter*ITERATIONS->r_offs), 0,
                 size, size, asize,
                 get, 0, ITERATIONS->n_sample, i,
                 dest, &defect);
}

}
}
