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
