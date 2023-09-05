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
#include "IMB_ze.h"

static ze_context_handle_t l0_context = ZE_NULL_HANDLE;
static ze_driver_handle_t l0_driver = ZE_NULL_HANDLE;
static ze_device_handle_t l0_device = ZE_NULL_HANDLE;
static ze_command_queue_handle_t l0_cq = ZE_NULL_HANDLE;
static ze_command_list_handle_t l0_cl = ZE_NULL_HANDLE;

int ze_initialize(char *dll_name)
{
    level_zero_init_functable_dll(dll_name);
    ZE_CHKERR(zeInit(0));

    uint32_t ze_driver_count = 1, ze_device_count = 1;
    ZE_CHKERR(zeDriverGet(&ze_driver_count, &l0_driver));
    ZE_CHKERR(zeDeviceGet(l0_driver, &ze_device_count, &l0_device));

    ze_context_desc_t ctxtDesc = {ZE_STRUCTURE_TYPE_CONTEXT_DESC, NULL, 0};
    ZE_CHKERR(zeContextCreate(l0_driver, &ctxtDesc, &l0_context));

    ze_command_queue_desc_t l0_cq_desc = {
        .flags = 0,
        .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
        .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL,
        .ordinal = 0 /* this must be less than device_properties.numAsyncComputeEngines */
    };
    ZE_CHKERR(zeCommandQueueCreate(l0_context, l0_device, &l0_cq_desc, &l0_cq));

    ze_command_list_desc_t l0_cl_desc = {.flags = 0};
    ZE_CHKERR(zeCommandListCreate(l0_context, l0_device, &l0_cl_desc, &l0_cl));
    return 0;
f_err:
    return 1;
}

int ze_memcpy(void *dst, const void *src, size_t sz)
{
    ZE_CHKERR(zeCommandListAppendMemoryCopy(l0_cl, dst, src, sz, ZE_NULL_HANDLE, 0, ZE_NULL_HANDLE));
    ZE_CHKERR(zeCommandListClose(l0_cl));
    ZE_CHKERR(zeCommandQueueExecuteCommandLists(l0_cq, 1, &l0_cl, ZE_NULL_HANDLE));
    ZE_CHKERR(zeCommandQueueSynchronize(l0_cq, UINT32_MAX));
    ZE_CHKERR(zeCommandListReset(l0_cl));
    return 0;
f_err:
    return 1;
}

void *ze_alloc(size_t size, char *where, MEM_ALLOC_TYPE mem_alloc_type)
{
    void *buf;
    switch (mem_alloc_type)
    {
        case MAT_DEVICE:
        {
            ze_device_mem_alloc_desc_t l0_device_mem_desc = {
                .flags = 0,
                .ordinal = 0 /* this must be less than count of zeDeviceGetMemoryProperties */
        };
            ZE_CHKERR(zeMemAllocDevice(l0_context, &l0_device_mem_desc, size, IMB_L0_MEM_ALIGNMENT, l0_device, &buf));
            break;
        }
        case MAT_HOST:
        {
            ze_host_mem_alloc_desc_t l0_host_mem_desc = {
                .flags = 0,
            };
            ZE_CHKERR(zeMemAllocHost(l0_context, &l0_host_mem_desc, size, IMB_L0_MEM_ALIGNMENT, &buf));
            break;
        }
        case MAT_SHARED:
        {
            ze_device_mem_alloc_desc_t l0_device_mem_desc = {
                .flags = 0,
                .ordinal = 0 /* this must be less than count of zeDeviceGetMemoryProperties */
            };
            ze_host_mem_alloc_desc_t l0_host_mem_desc = {
                .flags = 0,
            };
            ZE_CHKERR(zeMemAllocShared(l0_context, &l0_device_mem_desc, &l0_host_mem_desc, size, IMB_L0_MEM_ALIGNMENT, l0_device, &buf));
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

void ze_ass_buf(void *buf, int rank, size_t pos1, size_t pos2, int value)
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
    ze_memcpy(tmp_buf, buf + pos1, pos2 - pos1);
    IMB_ass_buf(tmp_buf, rank, pos1, pos2, value);
    ze_memcpy(buf + pos1, tmp_buf, pos2 - pos1);
    free(tmp_buf);
}

void ze_free(void **B, MEM_ALLOC_TYPE mem_alloc_type)
{
    if (B && *B)
    {
        if (l0_driver)
        {
            ZE_CHKERR_DO_NOT_FAIL(zeMemFree(l0_context, *B));
            *B = NULL;
        }
    }
}

#endif // GPU_ENABLE
