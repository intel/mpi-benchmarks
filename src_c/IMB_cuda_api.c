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
#include "IMB_cuda_api.h"

cuda_functable_t cuda_proxy = {
    NULL,
};

static const char *fn_names[] = {
    "cudaMalloc",
    "cudaMallocHost",
    "cudaMallocManaged",
    "cudaFree",
    "cudaFreeHost",
    "cudaMemcpyAsync",
    "cudaGetDeviceCount",
    "cudaStreamCreate",
    "cudaStreamDestroy",
    "cudaStreamSynchronize",
};

#define CUDA_FUNCTABLE_COUNT (sizeof(cuda_proxy) / sizeof(void *))

void cuda_init_functable_dll(const char *dll_name)
{
    void **sym_ptr = (void **)&cuda_proxy;

    void *handle = dlopen(dll_name, RTLD_LAZY | RTLD_GLOBAL);

    char *dl_error = dlerror();

    if (dl_error)
    {
        printf("%s\n", dl_error);
        exit(1);
    }

    size_t i;

    for (i = 0; i < CUDA_FUNCTABLE_COUNT; i++)
    {
        sym_ptr[i] = dlsym(handle, fn_names[i]);

        dl_error = dlerror();

        if (dl_error)
        {
            printf("%s\n", dl_error);
            exit(1);
        }

        if (sym_ptr[i] == NULL)
        {
            printf("Symbol %s is not available!\n", fn_names[i]);
            exit(1);
        }
    }
}

#endif // GPU_ENABLE
