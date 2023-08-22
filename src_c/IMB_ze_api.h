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

#ifndef IMB_ZE_API_H
#define IMB_ZE_API_H

#include <dlfcn.h>
#include "ze_api.h"

typedef struct level_zero_functable_t {
    typeof(zeInit)                            *zeInit;
    typeof(zeDriverGet)                       *zeDriverGet;
    typeof(zeMemAllocDevice)                  *zeMemAllocDevice;
    typeof(zeMemAllocHost)                    *zeMemAllocHost;
    typeof(zeMemAllocShared)                  *zeMemAllocShared;
    typeof(zeMemFree)                         *zeMemFree;
    typeof(zeDeviceGet)                       *zeDeviceGet;
    typeof(zeCommandQueueCreate)              *zeCommandQueueCreate;
    typeof(zeCommandQueueExecuteCommandLists) *zeCommandQueueExecuteCommandLists;
    typeof(zeCommandQueueSynchronize)         *zeCommandQueueSynchronize;
    typeof(zeCommandListCreate)               *zeCommandListCreate;
    typeof(zeCommandListCreateImmediate)      *zeCommandListCreateImmediate;
    typeof(zeCommandListAppendMemoryCopy)     *zeCommandListAppendMemoryCopy;
    typeof(zeCommandListReset)                *zeCommandListReset;
    typeof(zeCommandListClose)                *zeCommandListClose;
    typeof(zeContextCreate)                   *zeContextCreate;
} level_zero_functable_t;

extern level_zero_functable_t level_zero_proxy;

#define zeInit                            level_zero_proxy.zeInit
#define zeDriverGet                       level_zero_proxy.zeDriverGet
#define zeMemAllocDevice                  level_zero_proxy.zeMemAllocDevice
#define zeMemAllocHost                    level_zero_proxy.zeMemAllocHost
#define zeMemAllocShared                  level_zero_proxy.zeMemAllocShared
#define zeMemFree                         level_zero_proxy.zeMemFree
#define zeDeviceGet                       level_zero_proxy.zeDeviceGet
#define zeCommandQueueCreate              level_zero_proxy.zeCommandQueueCreate
#define zeCommandQueueExecuteCommandLists level_zero_proxy.zeCommandQueueExecuteCommandLists
#define zeCommandQueueSynchronize         level_zero_proxy.zeCommandQueueSynchronize
#define zeCommandListCreate               level_zero_proxy.zeCommandListCreate
#define zeCommandListCreateImmediate      level_zero_proxy.zeCommandListCreateImmediate
#define zeCommandListAppendMemoryCopy     level_zero_proxy.zeCommandListAppendMemoryCopy
#define zeCommandListReset                level_zero_proxy.zeCommandListReset
#define zeCommandListClose                level_zero_proxy.zeCommandListClose
#define zeContextCreate                   level_zero_proxy.zeContextCreate

void level_zero_init_functable_dll(const char *dll_name);

#endif /* IMB_ZE_API_H */
