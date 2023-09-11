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
