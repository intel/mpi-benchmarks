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



void IMB_err_hand(int ERR_IS_MPI, int ERR_CODE) {
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
    int i_break;

    if (!ERR_IS_MPI) {
        i_break = 1;
        fprintf(stderr, "Application error code %d occurred\n", ERR_CODE);
        switch (ERR_CODE) {
            case APPL_ERR_INVCASE: fprintf(stderr, "INVALID benchmark name\n");
                break;
            case APPL_ERR_MALLOC: fprintf(stderr, "Malloc failed \n");
                break;
            case APPL_ERR_CINFO: fprintf(stderr, "Invalid \"c_info\" setup\n");
                break;
        }
        ERR_CODE = MPI_ERR_INTERN;
    } else if (ERR_CODE != MPI_SUCCESS) {
        char aux_string[MPI_MAX_ERROR_STRING];
        int L;
        i_break = 1;
        fprintf(stderr, "MPI error  %d occurred\n", ERR_CODE);
        MPI_Error_string(ERR_CODE, aux_string, &L);
        fprintf(stderr, "%s\n", aux_string);
    }
    else i_break = 0;

    if (i_break)
        MPI_Abort(MPI_COMM_WORLD, ERR_CODE);
}

void IMB_errors_mpi(MPI_Comm * comm, int* ierr, ...) {
/*

                      Error handler callback for MPI-1 errors

Input variables:

-comm                 (type MPI_Comm *)
                      Communicator which is in error

-ierr                 (type int*)
                      MPI error code

*/
    IMB_err_hand(1, *ierr);
}

#ifdef EXT

void IMB_errors_win(MPI_Win * WIN, int* ierr, ...) {
/*

                      Error handler callback for onesided communications errors

Input variables:

-WIN                  (type MPI_Win *)
                      MPI Window which is in error

-ierr                 (type int*)
                      MPI error code

*/
    IMB_err_hand(1, *ierr);
}
#endif

#ifdef MPIIO

void IMB_errors_io(MPI_File * fh, int* ierr, ...) {
/*

                      Error handler callback for MPI-IO errors

Input variables:

-fh                   (type MPI_File *)
                      MPI File which is in error

-ierr                 (type int*)
                      MPI error code

*/
    IMB_err_hand(1, *ierr);
}
#endif




void IMB_init_errhand(struct comm_info* c_info) {
/*

                      Creates MPI error handler component of c_info by MPI_<>_create_errhandler

In/out variables:

-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information

                      Error handler component is created
                      (c_info->ERR for MPI-1; c_info->ERRW for EXT; c_info->ERRF for MPIIO)



*/
#ifdef SET_ERRH

    MPI_Errhandler_create(IMB_errors_mpi, &c_info->ERR);

#ifdef EXT
    MPI_Win_create_errhandler(IMB_errors_win, &c_info->ERRW);
#endif

#ifdef MPIIO
    MPI_File_create_errhandler(IMB_errors_io, &c_info->ERRF);
#endif

#endif
}

void IMB_set_errhand(struct comm_info* c_info) {
/*

                      Sets MPI error handler component of c_info by MPI_<>_set_errhandler

In/out variables:

-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information

                      Error handler component is set

*/
#ifdef SET_ERRH

    if (c_info->communicator != MPI_COMM_NULL)
        MPI_Errhandler_set(c_info->communicator, c_info->ERR);

#ifdef EXT
    if (c_info->WIN != MPI_WIN_NULL)
        MPI_Win_set_errhandler(c_info->WIN, c_info->ERRW);
#endif

#ifdef MPIIO
    if (c_info->fh != MPI_FILE_NULL)
        MPI_File_set_errhandler(c_info->fh, c_info->ERRF);
#endif

#endif
}

void IMB_del_errhand(struct comm_info* c_info) {
/*

                      Deletes MPI error handler component of c_info by MPI_Errhandler_free

In/out variables:

-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information

                      MPI error handler component of c_info is deleted

*/
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
