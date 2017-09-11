/*****************************************************************************
 *                                                                           *
 * Copyright (c) 2003-2016 Intel Corporation.                                *
 * All rights reserved.                                                      *
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

[2] Intel (R) MPI Benchmarks
    Users Guide and Methodology Description
    In 
    doc/IMB_Users_Guide.pdf

 File: IMB_parse_name_ext.c 

 Implemented functions: 

 IMB_get_def_cases;
 IMB_set_bmark;

 ***************************************************************************/





#include <string.h>

#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_bnames_rma.h"
#include "IMB_comments.h"

#include "IMB_prototypes.h"


int IMB_get_def_cases(char*** defc, char*** Gcmt)
{
    *defc = &DEFC[0];
    *Gcmt = &Gral_cmt[0];
    return (int) (sizeof(DEFC)/sizeof(char*));
}

int IMB_get_all_cases(char*** allc)
/*

                      
                      Initializes default benchmark names (defc) and accompanying
                      comments (Gcmt)
                      


In/out variables: 

-defc                 (type char***)                      
                      List of benchkark names (strings)
                      

-Gcmt                 (type char***)                      
                      List of general comments (strings)
                      


*/
{
    *allc = &ALLC[0];
    return (int) (sizeof(ALLC)/sizeof(char*));
}


void IMB_set_bmark(struct Bench* bmark)
{
    BTYPES type;
    
    bmark->bench_comments = NIL_COMMENT;
    bmark->N_Modes = 2;
    bmark->Ntimes = 1;
    bmark->reduction = 0;
    
    bmark->RUN_MODES[0].AGGREGATE=0;
    bmark->RUN_MODES[1].AGGREGATE=1;
    bmark->RUN_MODES[0].NONBLOCKING =0;
    bmark->RUN_MODES[1].NONBLOCKING =0;
    bmark->RUN_MODES[0].BIDIR =0;
    bmark->RUN_MODES[1].BIDIR =0;
    
    
    if (!strcmp(bmark->name,"unidir_put"))
    { 
        bmark->Benchmark = IMB_rma_single_put;
        type = SingleTransfer;
        bmark->access = put;
    }
    else if (!strcmp(bmark->name,"unidir_get"))
    { 
        bmark->Benchmark = IMB_rma_single_get;
        type = SingleTransfer;
        bmark->access = get;
    }
    else if (!strcmp(bmark->name,"bidir_put"))
    { 
        bmark->Benchmark = IMB_rma_single_put;
        type = SingleTransfer;
        bmark->access = put;
        bmark->RUN_MODES[0].BIDIR = 1;
        bmark->RUN_MODES[1].BIDIR = 1;

    }
    else if (!strcmp(bmark->name,"bidir_get"))
    { 
        bmark->Benchmark = IMB_rma_single_get;
        type = SingleTransfer;
        bmark->access = get;
        bmark->RUN_MODES[0].BIDIR = 1;
        bmark->RUN_MODES[1].BIDIR = 1;

    }
    else if (!strcmp(bmark->name,"put_local"))
    { 
        bmark->Benchmark = IMB_rma_put_local;
        type = SingleTransfer;
        bmark->access = put;
    }
    else if (!strcmp(bmark->name,"get_local"))
    { 
        bmark->Benchmark = IMB_rma_get_local;
        type = SingleTransfer;
        bmark->access = get;
    }
    else if (!strcmp(bmark->name,"put_all_local"))
    { 
        bmark->N_Modes = 1;
        bmark->RUN_MODES[0].AGGREGATE=-1;
        bmark->Benchmark = IMB_rma_put_all_local;
        type = MultPassiveTransfer;
        bmark->access = put;
    }
    else if (!strcmp(bmark->name,"get_all_local"))
    { 
        bmark->N_Modes = 1;
        bmark->RUN_MODES[0].AGGREGATE=-1;
        bmark->Benchmark = IMB_rma_get_all_local;
        type = MultPassiveTransfer;
        bmark->access = get;
    }
    else if (!strcmp(bmark->name,"one_put_all"))
    { 
        bmark->N_Modes = 1;
        bmark->Benchmark = IMB_rma_put_all;
        type = MultPassiveTransfer;
        bmark->access = put;
        bmark->RUN_MODES[0].AGGREGATE=-1;

    }
    else if (!strcmp(bmark->name,"one_get_all"))
    { 
        bmark->N_Modes = 1;
        bmark->Benchmark = IMB_rma_get_all;
        type = MultPassiveTransfer;
        bmark->access = get;
        bmark->RUN_MODES[0].AGGREGATE=-1;

    }
    else if (!strcmp(bmark->name,"all_put_all"))
    { 
        bmark->N_Modes = 1;
        bmark->Benchmark = IMB_rma_put_all;
        type = Collective;
        bmark->access = put;
        bmark->RUN_MODES[0].AGGREGATE=-1;
    }
    else if (!strcmp(bmark->name,"all_get_all"))
    { 
        bmark->N_Modes = 1;
        bmark->Benchmark = IMB_rma_get_all;
        type = Collective;
        bmark->access = get;
        bmark->RUN_MODES[0].AGGREGATE=-1;
    }
    else if (!strcmp(bmark->name,"exchange_put"))
    { 
        bmark->N_Modes = 1;
        bmark->Benchmark = IMB_rma_exchange_put;
        type = Collective;
        bmark->access = put;
        bmark->RUN_MODES[0].AGGREGATE=-1;
    }
    else if (!strcmp(bmark->name,"exchange_get"))
    { 
        bmark->N_Modes = 1;
        bmark->Benchmark = IMB_rma_exchange_get;
        type = Collective;
        bmark->access = get;
        bmark->RUN_MODES[0].AGGREGATE=-1;
    }
    else if (!strcmp(bmark->name,"accumulate"))
    { 
        bmark->Benchmark = IMB_rma_accumulate;
        type = SingleTransfer;
        bmark->access = put;
        bmark->reduction = 1;
    }
    else if (!strcmp(bmark->name,"get_accumulate"))
    { 
        bmark->Benchmark = IMB_rma_get_accumulate;
        type = SingleTransfer;
        bmark->access = put;
        bmark->reduction = 1;
    }
    else if (!strcmp(bmark->name,"fetch_and_op"))
    { 
        bmark->Benchmark = IMB_rma_fetch_and_op;
        type = SingleElementTransfer;
        bmark->access = put;
        bmark->reduction = 1;
    }
    else if (!strcmp(bmark->name,"compare_and_swap"))
    { 
        bmark->Benchmark = IMB_rma_compare_and_swap;
        type = SingleElementTransfer;
        bmark->access = put;
        bmark->bench_comments = Compare_and_swap_cmt;
    }
    else if (!strcmp(bmark->name,"truly_passive_put"))
    { 
        bmark->Benchmark = IMB_rma_passive_put;
        type = SingleTransfer;
        bmark->access = put;
        bmark->N_Modes = 1;
        bmark->Ntimes = 2;
        bmark->RUN_MODES[0].AGGREGATE=-1;
        bmark->RUN_MODES[0].NONBLOCKING=1;
        bmark->bench_comments = Truly_passive_put_cmt;
    }
    else 
    {
        bmark->RUN_MODES[0].type = BTYPE_INVALID;
        bmark->RUN_MODES[1].type = BTYPE_INVALID;
        return;
    }
    
    bmark->RUN_MODES[0].type = type;
    bmark->RUN_MODES[1].type = type;
    bmark->name[0] = toupper(bmark->name[0]);
}
