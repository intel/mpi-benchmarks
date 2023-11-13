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
#ifndef __cuda_h__
#define __cuda_h__

#include "stdbool.h"
#include <stdio.h>

#include "IMB_declare.h"
#include "IMB_benchmark.h"
#include "IMB_cuda_api.h"

static unsigned num_devices = 0;

#define CUDA_CHKERR_EXPL_FINALLY(f, fin, a) { cudaError_t _cuda_chkerr_err; \
    if ((_cuda_chkerr_err=(f), (fin), _cuda_chkerr_err) != cudaSuccess) { \
        fprintf(stderr, "ERROR: 0x%x returned from %s\n", _cuda_chkerr_err, #f); \
        a; \
    } \
}
#define CUDA_CHKERR_EXPL(f, a) CUDA_CHKERR_EXPL_FINALLY(f, 0, a)
#define CUDA_CHKERR_FINALLY(f, fin) CUDA_CHKERR_EXPL_FINALLY(f, fin, goto f_err)
#define CUDA_CHKERR(f) CUDA_CHKERR_EXPL(f, goto f_err)
#define CUDA_CHKERR_DO_NOT_FAIL(f) CUDA_CHKERR_EXPL_FINALLY(f, 0, (void)0)

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

cudaStream_t cuda_stream;

int cuda_initialize(char *dll_name);
void *cuda_alloc(size_t size, char *where, MEM_ALLOC_TYPE mem_alloc_type);
void cuda_free(void **B, MEM_ALLOC_TYPE mem_alloc_type);
void cuda_ass_buf(void *buf, int rank, size_t pos1, size_t pos2, int value);
int cuda_memcpy(void *dst, const void *src, size_t sz);

#endif
