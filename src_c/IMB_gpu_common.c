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
#include "IMB_gpu_common.h"

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
