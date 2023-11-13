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
    
 File: IMB_strgs.c 

 Implemented functions: 

 IMB_str;
 IMB_lwr;
 IMB_str_atoi;
 IMB_str_erase;

 ***************************************************************************/





#include <string.h>

#include "IMB_declare.h"

#include "IMB_prototypes.h"




char* IMB_str(const char* Bname) {
/*

                      Copies string Bname to new memory which is returned

Input variables:

-Bname                (type char*)
                      String to be copied

Return value          (type char*)
                      Copy of Bname with newly allocated memory

*/
    char* strg = (char*)IMB_v_alloc(1 + strlen(Bname), "str");

    if (strg)
        return strcpy(strg, Bname);
    else
        return NULL;
}


/***************************************************************************/
void IMB_lwr(char* Bname) {
/*

In/out variables:

-Bname                (type char*)
                      Uper case alphabetic characters are converted to lower case

*/
    int i;
    for (i = 0; i < strlen(Bname); i++)
        if (Bname[i] >= 'A' && Bname[i] <= 'Z')
            Bname[i] = Bname[i] - ('A' - 'a');
}



/***************************************************************************/



int IMB_str_atoi(char s[]) {
/*

                      Evaluates int value of a numeric string

Input variables:

-s                    (type char [])
                      String with only numeric characters

Return value          (type int)
                      Numeric value

*/

    int i, n, len;

    n = 0;
    len = strlen(s);

    for (i = 0; (s[i] >= '0' && s[i] <= '9') || s[i] == ' '; ++i) {
        if (s[i] != ' ')
            n = 10 * n + (s[i] - '0');
    }
    /* IMB_3.0 */
    if (len == 0 || i < len)
        n = -1;
    return n;
}


/* str_erase   */
void IMB_str_erase(char* string, int Nblnc) {
/*

                      Fills blancs into a string

Input variables:

-Nblnc                (type int)
                      #blancs to fill

In/out variables:

-string               (type char*)
                      Null terminated string with Nblnc many blancs

*/
    if (Nblnc > 0) {
        int i;
        for (i = 0; i < Nblnc; i++)
            string[i] = ' ';

        i = max(0, Nblnc);
        string[i] = '\0';
    }
}

