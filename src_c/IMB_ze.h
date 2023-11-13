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
    
 ***************************************************************************/

#ifndef __ze_h__
#define __ze_h__

#include <stddef.h>
#include "stdbool.h"
#include <stdio.h>

#include "IMB_declare.h"
#include "IMB_benchmark.h"
#include "IMB_ze_api.h"

#define IMB_L0_MEM_ALIGNMENT 64
#define ZE_NULL_HANDLE NULL /* just for verbosity */

#define ZE_CHKERR_EXPL_FINALLY(f, fin, a) { ze_result_t _ze_chkerr_err; \
    if ((_ze_chkerr_err=(f), (fin), _ze_chkerr_err) != ZE_RESULT_SUCCESS) { \
        fprintf(stderr, "ERROR: 0x%x returned from %s\n", _ze_chkerr_err, #f); \
        a; \
    } \
}
#define ZE_CHKERR_EXPL(f, a) ZE_CHKERR_EXPL_FINALLY(f, 0, a)
#define ZE_CHKERR_FINALLY(f, fin) ZE_CHKERR_EXPL_FINALLY(f, fin, goto f_err)
#define ZE_CHKERR(f) ZE_CHKERR_EXPL(f, goto f_err)
#define ZE_CHKERR_DO_NOT_FAIL(f) ZE_CHKERR_EXPL_FINALLY(f, 0, (void)0)

#define NZ_CHKERR_EXPL(f, a) if ((f) == 0) { \
    fprintf(stderr, "ERROR: %s failed\n", #f); \
    a; \
}
#define NZ_CHKERR(f) NZ_CHKERR_EXPL(f, goto f_err)

#define C_CHKERR_EXPL(f, a) if ((f) != 0) { \
    fprintf(stderr, "ERROR: %s failed\n", #f); \
    a; \
}
#define C_CHKERR(f) C_CHKERR_EXPL(f, goto f_err)

int ze_initialize(char *dll_name);
void *ze_alloc(size_t size, char *where, MEM_ALLOC_TYPE mem_alloc_type);
void ze_free(void **B, MEM_ALLOC_TYPE mem_alloc_type);
void ze_ass_buf(void *buf, int rank, size_t pos1, size_t pos2, int value);
int ze_memcpy(void *dst, const void *src, size_t sz);

#endif
