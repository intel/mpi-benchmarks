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

 File: IMB_init_transfer.c 

 Implemented functions: 

 IMB_init_transfer;
 IMB_close_transfer;

 ***************************************************************************/





#include "mpi.h"
#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_prototypes.h"




/* IMB 3.1 << */
void IMB_init_transfer(struct comm_info* c_info, struct Bench* Bmark, int size, MPI_Aint acc_size) {
/* >> IMB 3.1  */
/*

                          For IO  case: file splitting/view is set, file is opened
                          For EXT case: window is created and synchronized (MPI_Win_fence)

Input variables:

-Bmark                    (type struct Bench*)
                          (For explanation of struct Bench type:
                          describes all aspects of modes of a benchmark;
                          see [1] for more information)

                          Given benchmark

-size                     (type int)
                          (Only IO case): used to determine file view

IMB 3.1 <<
-acc_size                 (type int)
                          (Only EXT case): accumulate window size
>> IMB 3.1

In/out variables:

-c_info                   (type struct comm_info*)
                          Collection of all base data for MPI;
                          see [1] for more information

                          Corresponding components (File or Window related) are set

*/

#if defined MPIIO
#include <limits.h>
    int baslen;
    int ierr;
    int asize = (int) sizeof(assign_type);
    size_t pos1, pos2;

    if (c_info->File_rank < 0 || Bmark->access == no) return;

    if (size > 0) {
        IMB_get_rank_portion(c_info->File_rank, c_info->all_io_procs, size, asize,
                             &pos1, &pos2);
        baslen = (pos2 >= pos1) ? pos2 - pos1 + 1 : 0;
    } else {
        baslen = 0;
        pos2 = pos1 = 0;
    }

    if (c_info->view != MPI_DATATYPE_NULL)
        MPI_Type_free(&c_info->view);

    if (Bmark->fpointer == priv) {
        c_info->split.Locsize = baslen;
        c_info->split.Offset = 0;
        c_info->split.Totalsize = baslen;

        if (Bmark->access == put)
            IMB_set_buf(c_info, c_info->File_rank, 0,
                        (baslen > 0) ? baslen - 1 : 0,
                        1, 0);

        if (Bmark->access == get)
            IMB_set_buf(c_info, c_info->File_rank, 1, 0, 0,
                        (baslen > 0) ? baslen - 1 : 0);
    }

    if (Bmark->fpointer == indv_block || Bmark->fpointer == shared ||
        Bmark->fpointer == explic) {
        int bllen[1];

        MPI_Aint displ[1];
        MPI_Datatype types[1];

        bllen[0] = baslen;
        displ[0] = pos1;
        types[0] = c_info->etype;

        if (Bmark->fpointer == indv_block) {
            /* July 2002 fix V2.2.1: handle empty view case separately */
            if (baslen > 0) {
                MPI_Datatype new_type;
                /* end change */
                ierr = MPI_Type_create_struct(1, bllen, displ, types, &new_type);
                IMB_err_hand(1, ierr);
                ierr = MPI_Type_commit(&new_type);
                IMB_err_hand(1, ierr);

                ierr = MPI_Type_create_resized(new_type, 0, size, &c_info->view);
                IMB_err_hand(1, ierr);
                ierr = MPI_Type_commit(&c_info->view);
                IMB_err_hand(1, ierr);

                ierr = MPI_Type_free(&new_type);
                IMB_err_hand(1, ierr);

                c_info->filetype = c_info->view;

                /* July 2002 fix V2.2.1: handle empty case */
            } else
                c_info->filetype = c_info->etype;
            /* end change */
        }

        if (Bmark->access == put)
            IMB_set_buf(c_info, c_info->File_rank, 0, (baslen > 0) ? baslen - 1 : 0, 1, 0);

        if (Bmark->access == get)
            IMB_set_buf(c_info, c_info->File_rank, 1, 0, 0, (baslen > 0) ? baslen - 1 : 0);

        c_info->split.Locsize = bllen[0];
        c_info->split.Offset = pos1;
        c_info->split.Totalsize = size;
    }

    IMB_open_file(c_info);

#elif defined  EXT
    MPI_Aint sz;
    int s_size, r_size;

    if (Bmark->reduction) {
        MPI_Type_size(c_info->red_data_type, &s_size);
        r_size = s_size;
    } else {
        MPI_Type_size(c_info->s_data_type, &s_size);
        MPI_Type_size(c_info->r_data_type, &r_size);
    }

    if (c_info->rank >= 0) {
        IMB_user_set_info(&c_info->info);

        /* IMB 3.1 << */
        sz = acc_size;
        /* >> IMB 3.1  */

        if (Bmark->access == put) {
            MPI_ERRHAND(MPI_Win_create(c_info->r_buffer, sz, r_size, c_info->info,
                                       c_info->communicator, &c_info->WIN));
            MPI_ERRHAND(MPI_Win_fence(0, c_info->WIN));
        } else if (Bmark->access == get) {
            MPI_ERRHAND(MPI_Win_create(c_info->s_buffer, sz, s_size, c_info->info,
                                       c_info->communicator, &c_info->WIN));
            MPI_ERRHAND(MPI_Win_fence(0, c_info->WIN));
        }
    }
#elif defined RMA
    int s_size, r_size;

    if (Bmark->reduction) {
        MPI_Type_size(c_info->red_data_type, &s_size);
        r_size = s_size;
    } else {
        MPI_Type_size(c_info->s_data_type, &s_size);
        MPI_Type_size(c_info->r_data_type, &r_size);
    }

    if (c_info->rank >= 0) {
        IMB_user_set_info(&c_info->info);

        if (Bmark->access == put) {
            MPI_ERRHAND(MPI_Win_create(c_info->r_buffer, acc_size, r_size, c_info->info,
                                       c_info->communicator, &c_info->WIN));
        } else if (Bmark->access == get) {
            MPI_ERRHAND(MPI_Win_create(c_info->s_buffer, acc_size, r_size, c_info->info,
                                       c_info->communicator, &c_info->WIN));
        }
    }
#endif 

    IMB_set_errhand(c_info);
    err_flag = 0;
}


void IMB_close_transfer (struct comm_info* c_info, struct Bench* Bmark, int size) {
/*

       Closes / frees file / window components

Input variables:

-Bmark                (type struct Bench*)
                      (For explanation of struct Bench type:
                      describes all aspects of modes of a benchmark;
                      see [1] for more information)

Given benchmark

-size                 (type int)
                      (Only IO case): used to determine file view

In/out variables:

-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information

                      Corresponding components (File or Window related) are freed

*/
#ifdef MPIIO
    if (c_info->view != MPI_DATATYPE_NULL)
        MPI_Type_free(&c_info->view);

    if (c_info->File_rank >= 0 && Bmark->access != no && c_info->fh != MPI_FILE_NULL)
        MPI_File_close(&c_info->fh);

#else /*not MPIIO*/
#if (defined EXT || defined RMA)

    if (c_info->WIN != MPI_WIN_NULL)
        MPI_Win_free(&c_info->WIN);

#endif /*EXT || RMA*/
#endif /*MPIIO*/
}

