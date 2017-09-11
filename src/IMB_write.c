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

 File: IMB_write.c 

 Implemented functions: 

 IMB_write_shared;
 IMB_write_indv;
 IMB_write_expl;
 IMB_write_ij;
 IMB_iwrite_ij;

 ***************************************************************************/




#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_prototypes.h"


/*************************************************************************/


/*************************************************************************/

/* ===================================================================== */
/* 
IMB 3.1 changes
July 2007
Hans-Joachim Plum, Intel GmbH

- replace "int n_sample" by iteration scheduling object "ITERATIONS"
  (see => IMB_benchmark.h)

*/
/* ===================================================================== */

void IMB_write_shared(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                      MODES RUN_MODE, double* time)
/*

                      
                      MPI-IO benchmark kernel
                      Driver for write benchmarks with shared file pointers
                      


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
if( c_info->File_rank>=0 )
{
if( RUN_MODE->AGGREGATE )
IMB_write_ij(c_info, size, shared, RUN_MODE->type, 1, ITERATIONS->n_sample, 1, time);
else
IMB_write_ij(c_info, size, shared, RUN_MODE->type,  ITERATIONS->n_sample, 1, 0, time);

if( RUN_MODE->NONBLOCKING )
{
MPI_File_close(&c_info->fh);
IMB_open_file(c_info);

if( RUN_MODE->AGGREGATE )
IMB_iwrite_ij(c_info, size, shared, RUN_MODE->type, 1, ITERATIONS->n_sample, 1, 1, time+1);
else
IMB_iwrite_ij(c_info, size, shared, RUN_MODE->type,  ITERATIONS->n_sample, 1, 0, 1, time+1);
}
}
}

/*************************************************************************/



void IMB_write_indv(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                    MODES RUN_MODE, double* time)
/*

                      
                      MPI-IO benchmark kernel
                      Driver for write benchmarks with individual file pointers
                      


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
if( c_info->File_rank>=0 )
{
if( RUN_MODE->AGGREGATE )
IMB_write_ij(c_info, size, indv_block, RUN_MODE->type, 1, ITERATIONS->n_sample, 1, time);
else
IMB_write_ij(c_info, size, indv_block, RUN_MODE->type,  ITERATIONS->n_sample, 1, 0, time);

if( RUN_MODE->NONBLOCKING )
{
MPI_File_close(&c_info->fh);
IMB_open_file(c_info);

if( RUN_MODE->AGGREGATE )
IMB_iwrite_ij(c_info, size, indv_block, RUN_MODE->type, 1, ITERATIONS->n_sample, 1, 1, time+1);
else
IMB_iwrite_ij(c_info, size, indv_block, RUN_MODE->type,  ITERATIONS->n_sample, 1, 0, 1, time+1);
}
}
}

/*************************************************************************/



void IMB_write_expl(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                    MODES RUN_MODE, double* time)
/*

                      
                      MPI-IO benchmark kernel
                      Driver for write benchmarks with explicit offsets
                      


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
if( c_info->File_rank>=0 )
{
if( RUN_MODE->AGGREGATE )
IMB_write_ij(c_info, size, explicit, RUN_MODE->type, 1, ITERATIONS->n_sample, 1, time);
else
IMB_write_ij(c_info, size, explicit, RUN_MODE->type,  ITERATIONS->n_sample, 1, 0, time);

if( RUN_MODE->NONBLOCKING )
{
MPI_File_close(&c_info->fh);
IMB_open_file(c_info);

if( RUN_MODE->AGGREGATE )
IMB_iwrite_ij(c_info, size, explicit, RUN_MODE->type, 1, ITERATIONS->n_sample, 1, 1, time+1);
else
IMB_iwrite_ij(c_info, size, explicit, RUN_MODE->type,  ITERATIONS->n_sample, 1, 0, 1, time+1);
}
}
}


void IMB_write_ij(struct comm_info* c_info, int size, POSITIONING pos, 
                  BTYPES type, int i_sample, int j_sample, 
                  int time_inner, double* time)
/*

                      
                      Calls the proper write functions, blocking case
                      
                      (See IMB_read_ij for documentation of calling sequence)
                      


*/
{
int i, j;
int Locsize,Totalsize,Ioffs;
MPI_Status stat;
MPI_Offset Offset;

ierr = 0;

*time=0.;
if( c_info->File_rank >= 0 )
{

int (* GEN_File_write)(MPI_File fh, void* buf, int count, 
                       MPI_Datatype datatype, MPI_Status *status);
int (* GEN_File_write_shared)
                      (MPI_File fh, void* buf, int count, 
                       MPI_Datatype datatype, MPI_Status *status);
int (* GEN_File_write_at) 
                      (MPI_File fh, MPI_Offset offset, void* buf, int count, 
                       MPI_Datatype datatype, MPI_Status *status);

#ifdef CHECK
int chk_mode;

if( pos == shared && type != Collective) chk_mode = -3;
else                                     chk_mode = -2;

defect = 0.;
#endif

if (type == Collective )
  {
    GEN_File_write = MPI_File_write_all;
    GEN_File_write_shared = MPI_File_write_ordered;
    GEN_File_write_at = MPI_File_write_at_all;
#ifdef DEBUG
fprintf(unit,"Collective output\n");
#endif
  }
else
  {
    GEN_File_write = MPI_File_write;
    GEN_File_write_shared = MPI_File_write_shared;
    GEN_File_write_at = MPI_File_write_at;
#ifdef DEBUG
fprintf(unit,"Non collective output\n");
#endif
  }

Locsize = c_info->split.Locsize;
Totalsize = c_info->split.Totalsize;
Offset = (MPI_Offset)c_info->split.Offset;

/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
BLOCKING COLLECTIVE/NON COLLECTIVE OUTPUT CASES COMBINED
(function pointer GEN_File_write_XXX
either standard or collective MPI_File_write_XXX
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->File_comm);

if( !time_inner ) *time = MPI_Wtime();

for( i=0; i<i_sample; i++ )
{


if( time_inner ) *time = MPI_Wtime();

if( pos == indv_block )
for ( j=0; j<j_sample; j++ )
{

ierr=GEN_File_write(c_info->fh,c_info->s_buffer,Locsize,c_info->etype,&stat);
MPI_ERRHAND(ierr);

DIAGNOSTICS("Write standard ",c_info,c_info->s_buffer,Locsize,Totalsize,i+j,pos);

}

else if ( pos == explicit )
for ( j=0; j<j_sample; j++ )
{

Offset = c_info->split.Offset+(MPI_Offset)((i+j)*Totalsize);

ierr=GEN_File_write_at
     (c_info->fh, Offset, c_info->s_buffer,Locsize,c_info->etype,&stat);
MPI_ERRHAND(ierr);

DIAGNOSTICS("Write explicit ",c_info,c_info->s_buffer,Locsize,Totalsize,i+j,pos);

}

else if ( pos == shared )
for ( j=0; j<j_sample; j++ )
{

ierr=GEN_File_write_shared
     (c_info->fh,c_info->s_buffer,Locsize,c_info->etype,&stat);
MPI_ERRHAND(ierr);

DIAGNOSTICS("Write shared ",c_info,c_info->s_buffer,Locsize,Totalsize,i+j,pos);

}

// IMB_3.1 fix: use the following triple operation to assure write completion
MPI_File_sync(c_info->fh);
MPI_Barrier(c_info->File_comm);
MPI_File_sync(c_info->fh);

if( time_inner ) *time = (MPI_Wtime() - *time)/(i_sample*j_sample);

CHK_DIFF("Write_xxx",c_info, c_info->r_buffer, 0,
         Locsize, Totalsize, asize, 
         put, pos, j_sample, time_inner ? -1 : i,
         chk_mode, &defect);
CHK_STOP;

}

if( !time_inner ) *time = (MPI_Wtime() - *time)/(i_sample*j_sample);

}  /* end if (File_comm ) */
}



void IMB_iwrite_ij(struct comm_info* c_info, int size, POSITIONING pos, 
                   BTYPES type, int i_sample, int j_sample, 
                   int time_inner, int do_ovrlp, double* time)
/*

                      
                      Calls the proper write functions, non blocking case
                      
                      (See IMB_read_ij for documentation of calling sequence)
                      


*/
{
int i, j;
int Locsize,Totalsize,Ioffs;
MPI_Offset Offset;

MPI_Status*  STAT, stat;
MPI_Request* REQUESTS;

ierr = 0;

*time=0;

if( c_info->File_rank >= 0 )
{
#ifdef CHECK
int chk_mode;

if( pos == shared && type != Collective) chk_mode = -3;
else                                     chk_mode = -2;

defect = 0.;
#endif


Locsize = c_info->split.Locsize;
Totalsize = c_info->split.Totalsize;
Offset = (MPI_Offset)c_info->split.Offset;


if(type == Collective )

/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
NON BLOCKING COLLECTIVE OUTPUT CASES
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

{
for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->File_comm);

*time = MPI_Wtime();

if( pos == indv_block )

for ( j=0; j<i_sample*j_sample; j++ )
{

ierr=MPI_File_write_all_begin
  (c_info->fh,c_info->s_buffer,Locsize,c_info->etype);
MPI_ERRHAND(ierr);
DIAGNOSTICS("IWrite coll. ",c_info,c_info->s_buffer,Locsize,Totalsize,j,pos);

if( do_ovrlp )
IMB_cpu_exploit(TARGET_CPU_SECS,0);

ierr=MPI_File_write_all_end
  (c_info->fh,c_info->s_buffer,&stat);
MPI_ERRHAND(ierr);

}

else if ( pos == explicit )

for ( j=0; j<i_sample*j_sample; j++ )
{

Offset = c_info->split.Offset+(MPI_Offset)(j*Totalsize);

ierr=MPI_File_write_at_all_begin
  (c_info->fh,Offset,c_info->s_buffer,Locsize,c_info->etype);
MPI_ERRHAND(ierr);

DIAGNOSTICS("IWrite expl coll. ",c_info,c_info->s_buffer,Locsize,Totalsize,j,pos);

if( do_ovrlp )
IMB_cpu_exploit(TARGET_CPU_SECS,0);

ierr=MPI_File_write_at_all_end
  (c_info->fh,c_info->s_buffer,&stat);
MPI_ERRHAND(ierr);


}

else if ( pos == shared )

for ( j=0; j<i_sample*j_sample; j++ )
{

ierr=MPI_File_write_ordered_begin
  (c_info->fh,c_info->s_buffer,Locsize,c_info->etype);
MPI_ERRHAND(ierr);

DIAGNOSTICS("IWrite shared coll. ",c_info,c_info->s_buffer,Locsize,Totalsize,j,pos);

if( do_ovrlp )
IMB_cpu_exploit(TARGET_CPU_SECS,0);

ierr=MPI_File_write_ordered_end
  (c_info->fh,c_info->s_buffer,&stat);
MPI_ERRHAND(ierr);

}

// IMB_3.1 fix: use the following triple operation to assure write completion
MPI_File_sync(c_info->fh);
MPI_Barrier(c_info->File_comm);
MPI_File_sync(c_info->fh);


*time = (MPI_Wtime() - *time)/(i_sample*j_sample);

CHK_DIFF("Coll. IWrite_xxx",c_info, c_info->r_buffer, 0,
         Locsize, Totalsize, asize, 
         put, pos, i_sample*j_sample, -1,
         chk_mode, &defect);
CHK_STOP;

}

else  /* type non Collective */

/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
NON BLOCKING NON COLLECTIVE OUTPUT CASES
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

{

REQUESTS = (MPI_Request*)IMB_v_alloc(j_sample*sizeof(MPI_Request), "IWrite_ij");
STAT     = (MPI_Status *)IMB_v_alloc(j_sample*sizeof(MPI_Status ), "IWrite_ij");

for( j=0; j<j_sample; j++ ) REQUESTS[j]=MPI_REQUEST_NULL;

for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->File_comm);

if( !time_inner ) *time = MPI_Wtime();

for( i=0; i<i_sample; i++ )
{


if( time_inner ) *time = MPI_Wtime();

if( pos == indv_block )
for ( j=0; j<j_sample; j++ )
{

ierr=MPI_File_iwrite(c_info->fh,c_info->s_buffer,Locsize,c_info->etype,&REQUESTS[j]);
MPI_ERRHAND(ierr);
DIAGNOSTICS("IWrite standard ",c_info,c_info->s_buffer,Locsize,Totalsize,i+j,pos);

}

else if ( pos == explicit )
for ( j=0; j<j_sample; j++ )
{

Offset = c_info->split.Offset+(MPI_Offset)((i+j)*Totalsize);

ierr=MPI_File_iwrite_at
  (c_info->fh,Offset,c_info->s_buffer,Locsize,c_info->etype,&REQUESTS[j]);
MPI_ERRHAND(ierr);

DIAGNOSTICS("IWrite expl ",c_info,c_info->s_buffer,Locsize,Totalsize,i+j,pos);


}

else if ( pos == shared )
for ( j=0; j<j_sample; j++ )
{

ierr=MPI_File_iwrite_shared
  (c_info->fh,c_info->s_buffer,Locsize,c_info->etype,&REQUESTS[j]);
MPI_ERRHAND(ierr);

DIAGNOSTICS("IWrite shared ",c_info,c_info->s_buffer,Locsize,Totalsize,i+j,pos);

}


if( do_ovrlp )
for ( j=0; j<j_sample; j++ )
IMB_cpu_exploit(TARGET_CPU_SECS,0);

if( j_sample == 1 )
MPI_Wait(REQUESTS,STAT);
else                 
MPI_Waitall(j_sample,REQUESTS,STAT);


// IMB_3.1 fix: use the following triple operation to assure write completion
MPI_File_sync(c_info->fh);
MPI_Barrier(c_info->File_comm);
MPI_File_sync(c_info->fh);

if( time_inner ) *time = (MPI_Wtime() - *time)/(i_sample*j_sample);

CHK_DIFF("IWrite_xxx",c_info, c_info->r_buffer, 0,
         Locsize, Totalsize, asize, 
         put, pos, j_sample, time_inner ? -1 : i,
         chk_mode, &defect);
CHK_STOP;

}
if( !time_inner ) *time = (MPI_Wtime() - *time)/(i_sample*j_sample);

IMB_v_free ((void**)&REQUESTS);
IMB_v_free ((void**)&STAT);    

}


}  /* end if (File_comm ) */
}
