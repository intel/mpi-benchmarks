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

/* NAMES OF BENCHMARKS (DEFAULT CASE)*/
char *DEFC[] = {
    "Unidir_put",
    "Unidir_get",
    "Bidir_put",
    "Bidir_get",
    "One_put_all",
    "One_get_all",
    "All_put_all",
    "All_get_all",
    "Put_local",
    "Put_all_local",
    "Exchange_put",
    "Exchange_get",
    "Accumulate",
    "Get_accumulate",
    "Fetch_and_op",
    "Compare_and_swap",
    "Truly_passive_put"
};


/* Get_local and Get_all_local are not included to the def
 * case, because they are supposed to be very similar to
 * Unidir_get and One_get_all correspondingly */
char *ALLC[] = {
    "Unidir_put",
    "Unidir_get",
    "Bidir_put",
    "Bidir_get",
    "One_put_all",
    "One_get_all",
    "All_put_all",
    "All_get_all",
    "Put_local",
    "Get_local",
    "Put_all_local",
    "Get_all_local",
    "Exchange_put",
    "Exchange_get",
    "Accumulate",
    "Get_accumulate",
    "Fetch_and_op",
    "Compare_and_swap",
    "Truly_passive_put"
};
