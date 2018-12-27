/*
*****************************************************************************
*                                                                           *
* Copyright 2016-2019 Intel Corporation.                                    *
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
*/

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

extern FILE* unit;
void imb_p2p_barrier(MPI_Comm comm);
void imb_p2p_pause();
void imb_p2p_pingpong();
void imb_p2p_pingping();
void imb_p2p_unirandom();
void imb_p2p_birandom();
void imb_p2p_corandom();
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
