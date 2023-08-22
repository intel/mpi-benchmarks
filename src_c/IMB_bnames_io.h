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
