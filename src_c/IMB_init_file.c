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

 File: IMB_init_file.c 

 Implemented functions: 

 IMB_init_file_content;
 IMB_init_file;
 IMB_free_file;
 IMB_del_file;
 IMB_open_file;

 ***************************************************************************/




#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_prototypes.h"




void IMB_init_file_content(void* BUF, int pos1, int pos2) {
/*

                      Initializes contents of a file for READ benchmarks

Input variables:

-pos1                 (type int)
-pos2                 (type int)
                      pos1, pos2: target positions (start/end) in file

In/out variables:

-BUF                  (type void*)
                      Content of buffer to be written to file between these positions

*/
    IMB_ass_buf(BUF, 0, pos1, pos2, 1);
}




/* << IMB 3.1 */
int IMB_init_file(struct comm_info* c_info, struct Bench* Bmark, struct iter_schedule* ITERATIONS, int NP) {
/* >> IMB 3.1 */
/*

Input variables:

-Bmark                (type struct Bench*)
                      (For explanation of struct Bench type:
                      describes all aspects of modes of a benchmark;
                      see [1] for more information)

                      Given file i/o benchmark

-ITERATIONS           (type struct iter_schedule *)
                      Repetition scheduling

-NP                   (type int)
                      Number of active processes

In/out variables:

-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information

                      MPI_File component is set accordingly

Return value          (type int)
                      Error code (identical with MPI error code if occurs)

*/
    int error = 0;
    int fnlen;
    int asize = (int) sizeof(assign_type);

    IMB_free_file(c_info);

    c_info->fh = MPI_FILE_NULL;
    c_info->etype = MPI_BYTE;
    MPI_Type_size(c_info->etype, &c_info->e_size);
    c_info->filetype = c_info->etype;

    IMB_user_set_info(&c_info->info);

    c_info->disp = (MPI_Offset)0;

    c_info->datarep = IMB_str("native");

    if (Bmark->RUN_MODES[0].type == SingleTransfer)
        c_info->all_io_procs = 1;
    else
        c_info->all_io_procs = c_info->num_procs;

    fnlen = 1 + strlen(FILENAME);
    /* July 2002 fix V2.2.1: group_mode >= 0 */
    if (c_info->group_mode >= 0)
        fnlen += 4;

    if (Bmark->fpointer == priv) {
        if (c_info->rank > c_info->all_io_procs - 1 || c_info->rank < 0) {
            c_info->File_comm = MPI_COMM_NULL;
            c_info->File_rank = -1;
            c_info->File_num_procs = 0;
        } else {
            c_info->File_comm = MPI_COMM_SELF;
            c_info->File_rank = 0;
            c_info->File_num_procs = 1;
        }

        if (c_info->File_rank >= 0) {

            fnlen += 4;

            c_info->filename = (char*)IMB_v_alloc(sizeof(char)*fnlen, "Init_File");

            /* July 2002 fix V2.2.1: group_mode >= 0 */
            if (c_info->group_mode >= 0)
                sprintf(c_info->filename, "%s_g%d_%d", FILENAME, c_info->group_no, c_info->w_rank);
            else
                sprintf(c_info->filename, "%s_%d", FILENAME, c_info->w_rank);

            c_info->amode = MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_UNIQUE_OPEN;
        }
    } else {
        if (c_info->communicator == MPI_COMM_NULL) {
            c_info->File_comm = MPI_COMM_NULL;
            c_info->File_rank = -1;
            c_info->File_num_procs = 0;
        } else {
            c_info->File_comm = c_info->communicator;
            c_info->File_rank = c_info->rank;
            c_info->File_num_procs = c_info->num_procs;
        }

        c_info->filename = (char*)IMB_v_alloc(sizeof(char)*fnlen, "Init_File");
        /* July 2002 fix V2.2.1: group_mode >= 0 */
        if (c_info->group_mode >= 0)
            sprintf(c_info->filename, "%s_g%d", FILENAME, c_info->group_no);
        else
            sprintf(c_info->filename, "%s", FILENAME);

        c_info->amode = MPI_MODE_CREATE | MPI_MODE_RDWR;
    }

    if (Bmark->access == no) return 0;

    IMB_del_file(c_info); // if exists

    if (c_info->File_rank == 0) {
        int ierr, total, i;
        int size = 0;
        MPI_Status stat;

        /* << IMB 3.1. fixes of size */
        if (c_info->n_lens > 0) {
            for (i = 0; i < c_info->n_lens; i++)
                size = max(size, c_info->msglen[i]);
        }
        if (size == 0)
            size = 1 << c_info->max_msg_log;

        total = max(size, ITERATIONS->overall_vol);

        if (ITERATIONS->overall_vol / size > MSGSPERSAMPLE)
            total = size*MSGSPERSAMPLE;

        /* >> IMB 3.1 */

        /* July 2002 fix V2.2.1: calculation of file sizes in "priv" case */
        if (Bmark->fpointer == priv) {
            int NP = c_info->all_io_procs;

            total = (total + NP - 1) / NP;

            if (size%NP)
                total += asize*MSGSPERSAMPLE;
        }
        /* July 2002 end fix */

        ierr = MPI_File_open(MPI_COMM_SELF, c_info->filename,
                             c_info->amode, c_info->info, &c_info->fh);
        IMB_err_hand(1, ierr);

        MPI_File_set_view(c_info->fh, (MPI_Offset)0,
                          c_info->etype, c_info->etype, c_info->datarep, c_info->info);

        /* July 2002 fix V2.2.1: size <-> total */
        ierr = MPI_File_set_size(c_info->fh, total);
        IMB_err_hand(1, ierr);

        if (Bmark->access == get) {
            /* Prepare File for input */
            int el_size = 1 << 20;
            int pos1, pos2;

            pos1 = 0;

            while (pos1 < total) {
                pos2 = min(total - 1, pos1 + el_size - 1);
                size = ((pos2 - pos1) / asize + 1)*asize;


                IMB_alloc_buf(c_info, "Init_File 1 ", size, 0);
                IMB_init_file_content(c_info->s_buffer, pos1, pos2);

                ierr = MPI_File_write(c_info->fh, c_info->s_buffer, pos2 - pos1 + 1, c_info->etype, &stat);
                IMB_err_hand(1, ierr);

                pos1 = pos2 + 1;
            }

            IMB_del_s_buf(c_info);
        }

        ierr = MPI_File_close(&c_info->fh);
        IMB_err_hand(1, ierr);
    }

    return error;
}

void IMB_free_file(struct comm_info * c_info) {
/*

In/out variables:

-c_info               (type struct comm_info *)
                      Collection of all base data for MPI;
                      see [1] for more information

                      File related components are free-d and reset to
                      NULL initialization

*/
    if (c_info->filename != (char*)NULL) IMB_v_free((void**)&c_info->filename);
    if (c_info->datarep != (char*)NULL) IMB_v_free((void**)&c_info->datarep);
    if (c_info->view != MPI_DATATYPE_NULL)
        MPI_Type_free(&c_info->view);
    if (c_info->info != MPI_INFO_NULL)
        MPI_Info_free(&c_info->info);
    if (c_info->fh != MPI_FILE_NULL)
        MPI_File_close(&c_info->fh);
    c_info->filename = (char*)NULL;
    c_info->datarep = (char*)NULL;
    c_info->view = MPI_DATATYPE_NULL;
    c_info->info = MPI_INFO_NULL;
    c_info->fh = MPI_FILE_NULL;
}

void IMB_del_file(struct comm_info* c_info) {
/*

In/out variables:

-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information

                      File associated to MPI_File component is erased from disk

*/
    if (c_info->File_comm != MPI_COMM_NULL) {
        if (c_info->fh != MPI_FILE_NULL)
            MPI_File_close(&c_info->fh);
        MPI_Barrier(c_info->File_comm);

        if (c_info->filename != (char*)NULL) {
            if (c_info->File_rank == 0) {
                // touch file
                MPI_File_open(MPI_COMM_SELF, c_info->filename,
                                     c_info->amode, MPI_INFO_NULL, &c_info->fh);

                if (c_info->fh != MPI_FILE_NULL)
                    MPI_File_close(&c_info->fh);

                /* IMB_3.0: simplify file deletion */
                MPI_File_delete(c_info->filename, MPI_INFO_NULL);
            }
        }
        MPI_Barrier(c_info->File_comm);
    }
}

void IMB_open_file(struct comm_info* c_info) {
/*

In/out variables:

-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information

                      File associated to MPI_File component is opened, view is set

*/
    if (c_info->File_comm != MPI_COMM_NULL) {
        MPI_ERRHAND(MPI_File_open(c_info->File_comm, c_info->filename,
                                  c_info->amode, c_info->info, &c_info->fh));

        MPI_ERRHAND(MPI_File_set_view(c_info->fh, c_info->disp, c_info->etype,
                                      c_info->filetype, c_info->datarep, c_info->info));
    }
}
