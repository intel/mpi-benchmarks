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
#ifdef GPU_ENABLE
#include "IMB_gpu_common.h"

gpu_backend IMB_gpu_backend;

void gpu_initialize()
{
    char dll_name_cuda[] = "libcudart.so";

    char dll_name_ze[] = "libze_loader.so.1";

    if (dlopen(dll_name_cuda, RTLD_LAZY | RTLD_GLOBAL)) {
#ifdef CUDA_INCLUDE_DIR
        cuda_initialize(dll_name_cuda);
        IMB_gpu_backend.IMB_gpu_backend_type = GPU_BACKEND_CUDA;
        IMB_gpu_backend.IMB_gpu_alloc = cuda_alloc;
        IMB_gpu_backend.IMB_gpu_free = cuda_free;
        IMB_gpu_backend.IMB_gpu_ass_buf = cuda_ass_buf;
        IMB_gpu_backend.IMB_gpu_memcpy = cuda_memcpy;
#endif
    } else {
        if (dlopen(dll_name_ze, RTLD_LAZY | RTLD_GLOBAL)) {
#ifdef ZE_INCLUDE_DIR
            ze_initialize(dll_name_ze);
            IMB_gpu_backend.IMB_gpu_backend_type = GPU_BACKEND_ZE;
            IMB_gpu_backend.IMB_gpu_alloc = ze_alloc;
            IMB_gpu_backend.IMB_gpu_free = ze_free;
            IMB_gpu_backend.IMB_gpu_ass_buf = ze_ass_buf;
            IMB_gpu_backend.IMB_gpu_memcpy = ze_memcpy;
#endif
        } else {
            fprintf(stderr,"Neither CUDA nor Level Zero is available! \nPlease put path to gpu library in LD_LIBRARY_PATH\n");
            exit(1);
        }
    }
}

int gpu_memcpy(void *dst, const void *src, size_t sz)
{
    return IMB_gpu_backend.IMB_gpu_memcpy(dst, src, sz);
}
#endif //GPU_ENABLE
