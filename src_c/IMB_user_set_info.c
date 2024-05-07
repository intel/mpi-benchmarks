/****************************************************************************
*                                                                           *
* Copyright (C) 2024 Intel Corporation                                      *
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
    
 File: IMB_user_set_info.c 

 Implemented functions: 

 IMB_user_set_info;

 ***************************************************************************/





#include "mpi.h"

#include "IMB_declare.h"

#include "IMB_prototypes.h"




void IMB_user_set_info(MPI_Info* opt_info) {
/*



In/out variables:

-opt_info             (type MPI_Info*)
                      Is set. Default is MPI_INFO_NULL, everything beyond
                      this is user decision and system dependent.



*/
#ifdef MPIIO
    /*
    Set info for all MPI I/O functions
    */

    *opt_info = MPI_INFO_NULL;

#endif

#ifdef EXT
    /*
    Set info for all MPI_Win_create calls
    */

    *opt_info = MPI_INFO_NULL;

#endif

#ifdef RMA

    *opt_info = MPI_INFO_NULL;

#endif

#ifdef MPI4
    /*
    Set info for all MPI_***_init functions, where ***=collective_operation
    */

    *opt_info = MPI_INFO_NULL;

#endif

}
