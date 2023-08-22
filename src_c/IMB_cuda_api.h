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
#ifndef CUDA_API_H
#define CUDA_API_H

#include "cuda_runtime_api.h"
#include "driver_types.h"
#include <dlfcn.h>

/* *INDENT-OFF* */
/* The current function names are from CUDA 12.0 */
typedef struct cuda_functable_t {
    cudaError_t (*cudaMalloc)(void **, size_t);
    cudaError_t (*cudaMallocHost)(void **, size_t);
    cudaError_t (*cudaMallocManaged)(void **, size_t, unsigned int);
    typeof(cudaFree)                        *cudaFree;
    typeof(cudaFreeHost)                    *cudaFreeHost;
    typeof(cudaMemcpyAsync)                 *cudaMemcpyAsync;
    typeof(cudaGetDeviceCount)              *cudaGetDeviceCount;
    typeof(cudaStreamCreate)                *cudaStreamCreate;
    typeof(cudaStreamDestroy)               *cudaStreamDestroy;
    typeof(cudaStreamSynchronize)           *cudaStreamSynchronize;
} cuda_functable_t;

extern cuda_functable_t cuda_proxy;

#define cudaMalloc                          cuda_proxy.cudaMalloc
#define cudaMallocHost                      cuda_proxy.cudaMallocHost
#define cudaMallocManaged                   cuda_proxy.cudaMallocManaged
#define cudaFree                            cuda_proxy.cudaFree
#define cudaFreeHost                        cuda_proxy.cudaFreeHost
#define cudaMemcpyAsync                     cuda_proxy.cudaMemcpyAsync
#define cudaGetDeviceCount                  cuda_proxy.cudaGetDeviceCount
#define cudaStreamCreate                    cuda_proxy.cudaStreamCreate
#define cudaStreamDestroy                   cuda_proxy.cudaStreamDestroy
#define cudaStreamSynchronize               cuda_proxy.cudaStreamSynchronize

void cuda_init_functable_dll(const char *dll_name);

#endif /* CUDA_API_H */
