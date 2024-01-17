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
#ifndef IMB_GPU_COMMON_H
#define IMB_GPU_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "IMB_comm_info.h"
#ifdef ZE_INCLUDE_DIR
#include "IMB_ze.h"
#endif
#ifdef CUDA_INCLUDE_DIR
#include "IMB_cuda.h"
#endif

typedef enum gpu_backend_type_t {
    GPU_BACKEND_ZE = 0,
    GPU_BACKEND_CUDA,
} gpu_backend_type_t;

typedef struct gpu_backend {
    gpu_backend_type_t IMB_gpu_backend_type;
    void* (*IMB_gpu_alloc) (size_t, char*, MEM_ALLOC_TYPE);
    void  (*IMB_gpu_free) (void**,MEM_ALLOC_TYPE);
    void  (*IMB_gpu_ass_buf) (void*, int, size_t, size_t, int);
    int   (*IMB_gpu_memcpy) (void*, const void*, size_t);
} gpu_backend;

extern gpu_backend IMB_gpu_backend;

void gpu_initialize();

int gpu_memcpy(void *dst, const void *src, size_t sz);

#endif //IMB_GPU_COMMON_H
