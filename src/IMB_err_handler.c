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
    
 File: IMB_err_handler.c 

 Implemented functions: 

 IMB_err_hand;
 IMB_errors_mpi;
 IMB_errors_win;
 IMB_errors_io;
 IMB_init_errhand;
 IMB_set_errhand;
 IMB_del_errhand;

 ***************************************************************************/





#define nSET_ERRH

#include "IMB_declare.h"

#include "IMB_prototypes.h"

/*-------------------------------------------------------------*/



void IMB_err_hand(int ERR_IS_MPI, int ERR_CODE )
/*

                      
                      Handles input error code. 
                      Retrieves error string (MPI_Error_string) if is an MPI error code
                      Calls MPI_Abort
                      


Input variables: 

-ERR_IS_MPI           (type int)                      
                      Logical flag: error code belongs to MPI or not
                      

-ERR_CODE             (type int)                      
                      Input error code. If an MPI error code, the string is retrieved.
                      Anyway MPI_Abort is called
                      


*/
{
  int i_break;
  
  if(! ERR_IS_MPI )
    {
      i_break=1;
      fprintf(stderr,"Application error code %d occurred\n",ERR_CODE);
      switch(ERR_CODE)
	{
	case APPL_ERR_INVCASE: fprintf(stderr,"INVALID benchmark name\n");
	  break;
	case APPL_ERR_MALLOC : fprintf(stderr,"Malloc failed \n");
	  break;
	case APPL_ERR_CINFO  : fprintf(stderr,"Invalid \"c_info\" setup\n");
	  break;
	}
      ERR_CODE=MPI_ERR_INTERN;
    }
  else if ( ERR_CODE != MPI_SUCCESS )
    {
      char aux_string[MPI_MAX_ERROR_STRING];
      int L;
      i_break=1;
      fprintf(stderr,"MPI error  %d occurred\n",ERR_CODE);
      MPI_Error_string(ERR_CODE,aux_string, &L);
      fprintf(stderr,"%s\n",aux_string);
    }
  else i_break=0;
  
  if(i_break )
    { 
      MPI_Abort(MPI_COMM_WORLD, ERR_CODE);
    }
}




void IMB_errors_mpi(MPI_Comm * comm, int* ierr, ...)
/*

                      
                      Error handler callback for MPI-1 errors
                      


Input variables: 

-comm                 (type MPI_Comm *)                      
                      Communicator which is in error
                      

-ierr                 (type int*)                      
                      MPI error code
                      


*/
{
IMB_err_hand(1,*ierr);
}

#ifdef EXT

void IMB_errors_win(MPI_Win * WIN, int* ierr, ...)
/*

                      
                      Error handler callback for onesided communications errors
                      


Input variables: 

-WIN                  (type MPI_Win *)                      
                      MPI Window which is in error
                      

-ierr                 (type int*)                      
                      MPI error code
                      


*/
{
IMB_err_hand(1,*ierr);
}
#endif

#ifdef MPIIO

void IMB_errors_io (MPI_File * fh, int* ierr, ...)
/*

                      
                      Error handler callback for MPI-IO errors
                      


Input variables: 

-fh                   (type MPI_File *)                      
                      MPI File which is in error
                      

-ierr                 (type int*)                      
                      MPI error code
                      


*/
{
IMB_err_hand(1,*ierr);
}
#endif




void IMB_init_errhand(struct comm_info* c_info)
/*

                      
                      Creates MPI error handler component of c_info by MPI_<>_create_errhandler
                      


In/out variables: 

-c_info               (type struct comm_info*)                      
                      Collection of all base data for MPI;
                      see [1] for more information
                      
                      Error handler component is created 
                      (c_info->ERR for MPI-1; c_info->ERRW for EXT; c_info->ERRF for MPIIO)
                      


*/
{
#ifdef SET_ERRH

MPI_Errhandler_create(IMB_errors_mpi,&c_info->ERR);

#ifdef EXT
MPI_Win_create_errhandler(IMB_errors_win, &c_info->ERRW);
#endif

#ifdef MPIIO
MPI_File_create_errhandler(IMB_errors_io, &c_info->ERRF);
#endif

#endif
}




void IMB_set_errhand(struct comm_info* c_info)
/*

                      
                      Sets MPI error handler component of c_info by MPI_<>_set_errhandler
                      


In/out variables: 

-c_info               (type struct comm_info*)                      
                      Collection of all base data for MPI;
                      see [1] for more information
                      
                      Error handler component is set
                      


*/
{
#ifdef SET_ERRH

if( c_info->communicator != MPI_COMM_NULL )
MPI_Errhandler_set(c_info->communicator, c_info->ERR);

#ifdef EXT
if( c_info->WIN != MPI_WIN_NULL )
MPI_Win_set_errhandler(c_info->WIN, c_info->ERRW);
#endif

#ifdef MPIIO
if( c_info->fh != MPI_FILE_NULL )
MPI_File_set_errhandler(c_info->fh, c_info->ERRF);
#endif

#endif
}




void IMB_del_errhand(struct comm_info* c_info)
/*

                      
                      Deletes MPI error handler component of c_info by MPI_Errhandler_free
                      


In/out variables: 

-c_info               (type struct comm_info*)                      
                      Collection of all base data for MPI;
                      see [1] for more information
                      
                      MPI error handler component of c_info is deleted
                      


*/
{
#ifdef SET_ERRH
MPI_Errhandler_free(&c_info->ERR);

#ifdef EXT
MPI_Errhandler_free(&c_info->ERRW);
#endif

#ifdef MPIIO
MPI_Errhandler_free(&c_info->ERRF);
#endif

#endif
}
