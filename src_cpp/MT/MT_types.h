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

 ***************************************************************************/

#pragma once

#include <stdio.h>

#define THREAD_NUM_STR_LEN_MAX 16
#define THREAD_NUM_KEY "thread_id"

struct thread_local_data_t {
    int warmup;
    int repeat;
    MPI_Comm comm;
};

enum malopt_t {
    MALOPT_SERIAL,
    MALOPT_CONTINUOUS,
    MALOPT_PARALLEL
};

enum barropt_t {
    BARROPT_NOBARRIER,
    BARROPT_NORMAL,
    BARROPT_SPECIAL
};

static inline MPI_Comm duplicate_comm(int mode_multiple, int thread_num)
{
    char thread_num_str[THREAD_NUM_STR_LEN_MAX] = { 0 };
    MPI_Comm comm = MPI_COMM_WORLD, new_comm;
    MPI_Info info;

    if (mode_multiple) {
        MPI_Comm_dup(comm, &new_comm);
        MPI_Info_create(&info);
        snprintf(thread_num_str, THREAD_NUM_STR_LEN_MAX, "%d", thread_num);
        MPI_Info_set(info, THREAD_NUM_KEY, thread_num_str);
        MPI_Comm_set_info(new_comm, info);
        MPI_Info_free(&info);
        return new_comm;
    }
    return comm;
}
