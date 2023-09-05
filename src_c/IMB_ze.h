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

#ifndef __ze_h__
#define __ze_h__

#include <stddef.h>
#include "stdbool.h"
#include <stdio.h>

#include "IMB_declare.h"
#include "IMB_benchmark.h"
#include "IMB_ze_api.h"

#define IMB_L0_MEM_ALIGNMENT 64
#define ZE_NULL_HANDLE NULL /* just for verbosity */

#define ZE_CHKERR_EXPL_FINALLY(f, fin, a) { ze_result_t _ze_chkerr_err; \
    if ((_ze_chkerr_err=(f), (fin), _ze_chkerr_err) != ZE_RESULT_SUCCESS) { \
        fprintf(stderr, "ERROR: 0x%x returned from %s\n", _ze_chkerr_err, #f); \
        a; \
    } \
}
#define ZE_CHKERR_EXPL(f, a) ZE_CHKERR_EXPL_FINALLY(f, 0, a)
#define ZE_CHKERR_FINALLY(f, fin) ZE_CHKERR_EXPL_FINALLY(f, fin, goto f_err)
#define ZE_CHKERR(f) ZE_CHKERR_EXPL(f, goto f_err)
#define ZE_CHKERR_DO_NOT_FAIL(f) ZE_CHKERR_EXPL_FINALLY(f, 0, (void)0)

#define NZ_CHKERR_EXPL(f, a) if ((f) == 0) { \
    fprintf(stderr, "ERROR: %s failed\n", #f); \
    a; \
}
#define NZ_CHKERR(f) NZ_CHKERR_EXPL(f, goto f_err)

#define C_CHKERR_EXPL(f, a) if ((f) != 0) { \
    fprintf(stderr, "ERROR: %s failed\n", #f); \
    a; \
}
#define C_CHKERR(f) C_CHKERR_EXPL(f, goto f_err)

int ze_initialize(char *dll_name);
void *ze_alloc(size_t size, char *where, MEM_ALLOC_TYPE mem_alloc_type);
void ze_free(void **B, MEM_ALLOC_TYPE mem_alloc_type);
void ze_ass_buf(void *buf, int rank, size_t pos1, size_t pos2, int value);
int ze_memcpy(void *dst, const void *src, size_t sz);

#endif
