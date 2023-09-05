/*****************************************************************************
 *                                                                           *
 * Copyright Intel Corporation.                                              *
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
            printf("Error: Unknown buf type\n");
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

    static const int asize = (int)sizeof(assign_type);
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
                printf("Error: Unknown buf type\n");
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
