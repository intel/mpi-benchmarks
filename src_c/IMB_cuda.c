/****************************************************************************
*                                                                           *
* Copyright (C) 2024 Intel Corporation                                      *
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
#include "IMB_cuda.h"

static unsigned num_devices = 0;
cudaStream_t cuda_stream;

int cuda_initialize(char *dll_name)
{
    cuda_init_functable_dll(dll_name);
    CUDA_CHKERR(cudaGetDeviceCount((int *)&num_devices));
    CUDA_CHKERR(cudaStreamCreate(&cuda_stream));

    return 0;
f_err:
    return 1;
}

int cuda_memcpy(void *dst, const void *src, size_t sz)
{
    CUDA_CHKERR(cudaMemcpyAsync(dst, src, sz, cudaMemcpyDefault, cuda_stream));
    CUDA_CHKERR(cudaStreamSynchronize(cuda_stream));

    return 0;
f_err:
    return 1;
}

void *cuda_alloc(size_t size, char *where, MEM_ALLOC_TYPE mem_alloc_type)
{
    void *buf;
    switch (mem_alloc_type)
    {
        case MAT_DEVICE:
        {
            CUDA_CHKERR(cudaMalloc(&buf, size));
            break;
        }
        case MAT_HOST:
        {
            CUDA_CHKERR(cudaMallocHost(&buf, size));
            break;
        }
        case MAT_SHARED:
        {
            CUDA_CHKERR(cudaMallocManaged(&buf, size, cudaMemAttachGlobal));
            break;
        }
        default:
        {
            printf("ERROR: Unknown buf type\n");
            exit(1);
        }
    }

    return buf;
f_err:
    return NULL;
}

void cuda_ass_buf(void *buf, int rank, size_t pos1, size_t pos2, int value)
{
    /*

                          Assigns values to a buffer

    Input variables:

    -rank                 (type int)
                          Rank of calling process

    -pos1                 (type int)
    -pos2                 (type int)
                          Assignment between byte positions pos1, pos2

    -value                (type int)
                          1/0 for non-zero (defined in IMB_settings.h)/ zero value

    In/out variables:

    -buf                  (type void*)
                          Values assigned within given positions
    */
    if (pos2 <= pos1)
        return;

    char *tmp_buf = malloc(pos2 + 1 - pos1);
    cuda_memcpy(tmp_buf, buf + pos1, pos2 - pos1);
    IMB_ass_buf(tmp_buf, rank, pos1, pos2, value);
    cuda_memcpy(buf + pos1, tmp_buf, pos2 - pos1);
    free(tmp_buf);
}

void cuda_free(void **B, MEM_ALLOC_TYPE mem_alloc_type)
{
    if (B && *B)
    {
        switch (mem_alloc_type)
        {
            case MAT_DEVICE:
            {
                CUDA_CHKERR_DO_NOT_FAIL(cudaFree(*B));
                break;
            }
            case MAT_HOST:
            {
                CUDA_CHKERR_DO_NOT_FAIL(cudaFreeHost(*B));
                break;
            }
            case MAT_SHARED:
            {
                CUDA_CHKERR_DO_NOT_FAIL(cudaFree(*B));
                break;
            }
            default:
            {
                printf("ERROR: Unknown buf type\n");
                exit(1);
            }
        }
        if (cuda_stream)
        {
            CUDA_CHKERR_DO_NOT_FAIL(cudaStreamDestroy(cuda_stream));
            cuda_stream = NULL;
        }
        *B = NULL;
    }
}

#endif // GPU_ENABLE
