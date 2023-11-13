/****************************************************************************
*                                                                           *
* Copyright (C) 2020 Intel Corporation                                      *
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

#ifndef IMB_P2P_HEADER_INCLUDED
#define IMB_P2P_HEADER_INCLUDED

#include "mpi.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
#include <math.h>

#ifdef WIN_IMB
#define STRCASECMP(s1,s2) _stricmp((s1),(s2))
#else /* linux */
#include <strings.h>
#include <unistd.h>
#define STRCASECMP(s1,s2) strcasecmp((s1),(s2))
#endif

#define IMB_P2P_CACHE_LINE_LEN (64)
#define IMB_P2P_DEFAULT_MAX_MESSAGE_SIZE (4 * 1024 * 1024)
#define IMB_P2P_BARRIER_TAG 1000
#define IMB_P2P_MAX_MSG_LOG 47
#define IMB_P2P_PINGPONG "PingPong"
#define IMB_P2P_PINGPING "PingPing"
#define IMB_P2P_UNIRANDOM "Unirandom"
#define IMB_P2P_BIRANDOM "Birandom"
#define IMB_P2P_CORANDOM "Corandom"
#define IMB_P2P_STENCIL2D "Stencil2D"
#define IMB_P2P_STENCIL3D "Stencil3D"
#define IMB_P2P_SENDRECV_REPLACE "SendRecv_Replace"

extern FILE* unit;
void imb_p2p_barrier(MPI_Comm comm);
void imb_p2p_pause();
void imb_p2p_pingpong();
void imb_p2p_pingping();
void imb_p2p_unirandom();
void imb_p2p_birandom();
void imb_p2p_corandom();
void imb_p2p_stencil2d();
void imb_p2p_stencil3d();
void imb_p2p_sendrecv_replace();
void imb_p2p_print_benchmark_header(const char * name);
void imb_p2p_free_mem(void *ptr);
void* imb_p2p_alloc_mem(size_t size);
void* imb_p2p_realloc_mem(void *old_ptr, size_t old_size, size_t new_size);

typedef void(*imb_p2p_procedure_t)();

typedef struct imb_p2p_benchmark {
    const char * name;
    imb_p2p_procedure_t run;
} imb_p2p_benchmark_t;

typedef struct imb_p2p_configuration {
    int rank;
    int nranks;
    int pause_usec;
    struct {
        imb_p2p_benchmark_t * array;
        size_t length;
        size_t capacity;
    } benchmarks;
    struct {
        size_t * array;
        size_t length;
        size_t capacity;
        size_t min_size;
        size_t max_size;
    } messages;
    struct {
        size_t numerator;
        size_t max;
    } iter;
    struct {
        int is_touch_send_buff;
        int is_touch_recv_buff;
        size_t dummy;
    } payload;
} imb_p2p_configuration_t;

typedef struct imb_p2p_report {
    size_t number_of_messages;
    double time;
} imb_p2p_report_t;

extern imb_p2p_configuration_t imb_p2p_config;

static inline void touch_send_buff(size_t size, char * send_buffer) {
    if (imb_p2p_config.payload.is_touch_send_buff) {
        size_t dummy = imb_p2p_config.payload.dummy;
        size_t i = 0;
        while (i < size) {
            send_buffer[i] = (char)dummy;
            dummy++;
            i += IMB_P2P_CACHE_LINE_LEN;
        }
        imb_p2p_config.payload.dummy = dummy;
    }
}

static inline void touch_recv_buff(size_t size, char * recv_buffer) {
    if (imb_p2p_config.payload.is_touch_recv_buff) {
        size_t dummy = imb_p2p_config.payload.dummy;
        size_t i = 0;
        while (i < size) {
            dummy += recv_buffer[i];
            i += IMB_P2P_CACHE_LINE_LEN;
        }
        imb_p2p_config.payload.dummy = dummy;
    }
}

static inline void get_iters(size_t size, size_t * number_of_iterations, size_t * number_of_warm_up_iterations) {
    size_t n, w;
    if (size > 0) {
        n = round(((double)imb_p2p_config.iter.numerator) / size);
        if (n > imb_p2p_config.iter.max) {
            n = imb_p2p_config.iter.max;
        }
        if (n < 1) {
            n = 1;
        }
    } else {
        n = imb_p2p_config.iter.max;
    }
    w = (n / 10);
    if ((w == 0) && (n > 1)) {
        w = 1;
    }
    *number_of_iterations = n;
    *number_of_warm_up_iterations = w;
}

static inline uint32_t get_next_random(uint64_t *random_seed) {
    uint64_t n = *random_seed;
    n = n * 7560690468208412377ull + 4343336064253945357ull;
    *random_seed = n;
    return (n >> 24);
}

#endif /* IMB_P2P_HEADER_INCLUDED */
