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

#include "IMB_bnames_ext.h"
#include "IMB_comments.h"

#include "IMB_prototypes.h"


int IMB_get_def_cases(char*** defc, char*** Gcmt)
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


void IMB_set_bmark(struct Bench* Bmark)
/*



In/out variables: 

-Bmark                (type struct Bench*)                      
                      (For explanation of struct Bench type:
                      describes all aspects of modes of a benchmark;
                      see [1] for more information)
                      
                      On input, only the name of the benchmark is set.
                      On output, all necessary run modes are set accordingly
                      


*/
{
    BTYPES type;
#if 0
    int index;

    IMB_get_def_index(&index,Bmark->name);

    if( index < 0 )
    {
	Bmark->RUN_MODES[0].type=BTYPE_INVALID;
	Bmark->RUN_MODES[1].type=BTYPE_INVALID;
	return;
    }
#endif

    Bmark->RUN_MODES[0].AGGREGATE=1;
    Bmark->RUN_MODES[1].AGGREGATE=0;

    Bmark->RUN_MODES[0].NONBLOCKING =0;
    Bmark->RUN_MODES[1].NONBLOCKING =0;

    Bmark->N_Modes = 2;

    Bmark->reduction = 0;

    Bmark->Ntimes = 1;

    if (!strcmp(Bmark->name,"unidir_get"))
    { 
	strcpy(Bmark->name,"Unidir_Get");
	Bmark->Benchmark = IMB_unidir_get;
	Bmark->bench_comments = &Unidir_Get_cmt[0];
	type = SingleTransfer;
	Bmark->access = get;
    }
    else if (!strcmp(Bmark->name,"unidir_put"))
    { 
	strcpy(Bmark->name,"Unidir_Put");
	Bmark->Benchmark = IMB_unidir_put;
	Bmark->bench_comments = &Unidir_Put_cmt[0];
	type = SingleTransfer;
	Bmark->access = put;
    }
    else if (!strcmp(Bmark->name,"bidir_get"))
    { 
	strcpy(Bmark->name,"Bidir_Get");
	Bmark->Benchmark = IMB_bidir_get;
	Bmark->bench_comments = &Bidir_Get_cmt[0];
	type = SingleTransfer;
	Bmark->access = get;
    }
    else if (!strcmp(Bmark->name,"bidir_put"))
    { 
	strcpy(Bmark->name,"Bidir_Put");
	Bmark->Benchmark = IMB_bidir_put;
	Bmark->bench_comments = &Bidir_Put_cmt[0];
	type = SingleTransfer;
	Bmark->access = put;
    }
    else if (!strcmp(Bmark->name,"accumulate"))
    { 
	strcpy(Bmark->name,"Accumulate");
	Bmark->Benchmark = IMB_accumulate;
	Bmark->bench_comments = &Accumulate_cmt[0];
	type = Collective;
	Bmark->access = put;
	Bmark->reduction = 1;
    }
    else if (!strcmp(Bmark->name,"window"))
    { 
	strcpy(Bmark->name,"Window");
	Bmark->Benchmark = IMB_window;
	Bmark->bench_comments = &Window_cmt[0];
	Bmark->RUN_MODES[0].AGGREGATE=-1;
	Bmark->N_Modes = 1;
	type = Collective;
	Bmark->access = no;
	Bmark->reduction = 1;
    }
    else 
    {
	type = BTYPE_INVALID;
    }
    Bmark->RUN_MODES[0].type=type;
    Bmark->RUN_MODES[1].type=type;

}
