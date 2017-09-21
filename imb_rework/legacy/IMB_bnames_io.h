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
    
 ***************************************************************************/




#include "IMB_benchmark.h"

/* NAMES OF BENCHMARKS (RUNNING BY DEFAULT)*/
char *DEFC[] = {
   "S_Write_Indv",
   "S_IWrite_Indv",
   "S_Write_Expl",
   "S_IWrite_Expl",
   "P_Write_Indv",
   "P_IWrite_Indv",
   "P_Write_Shared",
   "P_IWrite_Shared",
   "P_Write_Priv",
   "P_IWrite_Priv",
   "P_Write_Expl",
   "P_IWrite_Expl",
   "C_Write_Indv",
   "C_IWrite_Indv",
   "C_Write_Shared",
   "C_IWrite_Shared",
   "C_Write_Expl",
   "C_IWrite_Expl",
   "S_Read_Indv",
   "S_IRead_Indv",
   "S_Read_Expl",
   "S_IRead_Expl",
   "P_Read_Indv",
   "P_IRead_Indv",
   "P_Read_Shared",
   "P_IRead_Shared",
   "P_Read_Priv",
   "P_IRead_Priv",
   "P_Read_Expl",
   "P_IRead_Expl",
   "C_Read_Indv",
   "C_IRead_Indv",
   "C_Read_Shared",
   "C_IRead_Shared",
   "C_Read_Expl",
   "C_IRead_Expl",
   "Open_Close"
};

/* NAMES OF BENCHMARKS (ALL POSSIBLE CASES)*/
char *ALLC[] = {
   "S_Write_Indv",
   "S_IWrite_Indv",
   "S_Write_Expl",
   "S_IWrite_Expl",
   "P_Write_Indv",
   "P_IWrite_Indv",
   "P_Write_Shared",
   "P_IWrite_Shared",
   "P_Write_Priv",
   "P_IWrite_Priv",
   "P_Write_Expl",
   "P_IWrite_Expl",
   "C_Write_Indv",
   "C_IWrite_Indv",
   "C_Write_Shared",
   "C_IWrite_Shared",
   "C_Write_Expl",
   "C_IWrite_Expl",
   "S_Read_Indv",
   "S_IRead_Indv",
   "S_Read_Expl",
   "S_IRead_Expl",
   "P_Read_Indv",
   "P_IRead_Indv",
   "P_Read_Shared",
   "P_IRead_Shared",
   "P_Read_Priv",
   "P_IRead_Priv",
   "P_Read_Expl",
   "P_IRead_Expl",
   "C_Read_Indv",
   "C_IRead_Indv",
   "C_Read_Shared",
   "C_IRead_Shared",
   "C_Read_Expl",
   "C_IRead_Expl",
   "Open_Close"
};
