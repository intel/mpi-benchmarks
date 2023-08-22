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
