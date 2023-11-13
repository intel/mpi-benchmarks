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

 File: IMB_benchlist.c 

 Implemented functions: 

 IMB_list_names;
 IMB_get_def_index;
 IMB_construct_blist;
 IMB_destruct_blist;
 IMB_print_blist;

 ***************************************************************************/





#include <stdio.h>

#include "IMB_declare.h"

#include "IMB_benchmark.h"

#include "IMB_prototypes.h"

char * NIL_COMMENT[] = { NULL };



#if 0
void IMB_list_names(char* Bname, int** List) {
/*



Input variables:

-Bname                (type char*)
                      Input benchmark name (or "all" for all available benchmarks)


In/out variables:

-List                 (type int**)
                      Auxiliary list of internal numbering for input benchmark(s)


*/
    char** def_cases, **General_cmt;

    IMB_get_def_cases(&def_cases, &General_cmt);

    if (!strcmp(Bname, "all")) {
        int Ndeflt = -1, n;

        while (def_cases[++Ndeflt]);

        IMB_i_alloc(int, *List, Ndeflt + 1, "List_Names");

        for (n = 0; n < Ndeflt; n++)
            (*List)[n] = n;
        /* IMB_3.0
              (*List)[Ndeflt] = -1;
              */
        (*List)[Ndeflt] = LIST_END;
    } else {
        IMB_i_alloc(int, *List, 2, "List_Names");
        IMB_get_def_index(*List, Bname);
        /* IMB_3.0
              (*List)[1]=-1;
              */

        (*List)[1] = LIST_END;
    }
}
#endif

int IMB_get_bmark_index(char* name) {
/*

Input variables:

-name                 (type char*)
                      Input benchmark name

*/
    char** all_cases;
    int ncases, index;

    ncases = IMB_get_all_cases(&all_cases);

    for (index = 0; index < ncases; index++)
    {
        char* TMP1;
        char* TMP2;
        int  iret;

        TMP1 = IMB_str(all_cases[index]);
        if (TMP1 == NULL) exit(1);

        TMP2 = IMB_str(name);
        if (TMP2 == NULL) exit(1);

        iret = IMB_strcasecmp(TMP1, TMP2);

        IMB_v_free((void**)&TMP1);
        IMB_v_free((void**)&TMP2);

        if (iret == 0) break;

    } /* for */

    return (index < ncases) ? index : LIST_INVALID;
}

void IMB_construct_blist_default(struct Bench** P_BList) {
    struct Bench* Bmark;
    char** def_cases, **General_cmt;
    int i;
    int NumBench = IMB_get_def_cases(&def_cases, &General_cmt);

    *P_BList = (struct Bench*)IMB_v_alloc((1 + NumBench) * sizeof(struct Bench), "Construct_Blist 1");

    for (i = 0; i < NumBench; i++)
    {
        Bmark = &(*P_BList)[i];
        Bmark->name = IMB_str(def_cases[i]);

        if (Bmark->name == NULL) exit(1);

        IMB_lwr(Bmark->name);

        Bmark->bench_comments = &NIL_COMMENT[0];
        Bmark->scale_time     = 1.0;
        Bmark->scale_bw       = 1.0;
        Bmark->success        = 1;
        Bmark->sample_failure = 0;
        IMB_set_bmark(Bmark);

    }

    (*P_BList)[NumBench].name = NULL;
}


void IMB_construct_blist(struct Bench* Bmark, const char* bname) {
    Bmark->name = IMB_str((char *)bname);

    if (Bmark->name == NULL) exit(1);

    IMB_lwr(Bmark->name);

    Bmark->bench_comments = &NIL_COMMENT[0];
    Bmark->scale_time     = 1.0;
    Bmark->scale_bw       = 1.0;
    Bmark->success        = 1;
    Bmark->sample_failure = 0;
    IMB_set_bmark(Bmark);
}

#if 0
void IMB_construct_blist(struct Bench** P_BList, int n_args, char* name)
/*

                      Sets up the list of requested benchmarks
                      (represented as list of struct Bench structures).
                      In one call, 1 benchmark is included.

Input variables:

-n_args               (type int)
                      Overall number of benchmarks to be run (0 means "all")

-name                 (type char*)
                      Name of benchmark to be included in list


Output variables:

-P_BList              (type struct Bench**)
                      (For explanation of struct Bench type:
                      describes all aspects of modes of a benchmark;
                      see [1] for more information)

                      Updated benchmark list

*/
{

    static int Ndeflt = -1;
    static int n_cases;

    struct Bench* Bmark;
    char* Bname;

    char** def_cases, **General_cmt;

    int* List;
    int plc;

    IMB_get_def_cases(&def_cases, &General_cmt);

    if (Ndeflt < 0) {
        Ndeflt = -1;
        while (def_cases[++Ndeflt]);

        *P_BList = (struct Bench*)IMB_v_alloc((1 + n_args + Ndeflt) * sizeof(struct Bench), "Construct_Blist 1");

        n_cases = 0;
    }

    Bname = IMB_str(name);
    IMB_lwr(Bname);

    IMB_list_names(Bname, &List);

    /* IMB_3.0
     for( plc=0; List[plc]>=0 ; plc++ )
     */
    for (plc = 0; List[plc] != LIST_END; plc++) {
        Bmark = (*P_BList) + n_cases;

        /* IMB_3.0 */
        if (List[plc] >= 0) {
            Bmark->name = IMB_str(def_cases[List[plc]]);
        }
        else {
            Bmark->name = IMB_str(Bname);
        }
        IMB_lwr(Bmark->name);

        Bmark->bench_comments = &NIL_COMMENT[0];
        Bmark->scale_time     = 1.0;
        Bmark->scale_bw       = 1.0;
        Bmark->success        = 1;
        /* IMB 3.1 << */
        Bmark->sample_failure = 0;
        /* >> IMB 3.1  */

        IMB_set_bmark(Bmark);

        /* IMB_3.0
             if( Bmark->RUN_MODES[0].type == BTYPE_INVALID ) strcpy(Bmark->name,name);
             */
        n_cases++;
    } /* for*/

    (*P_BList)[n_cases].name = NULL;

    IMB_v_free((void**)&Bname);
    IMB_v_free((void**)&List);
}
#endif



void IMB_destruct_blist(struct Bench ** P_BList) {
/*

                      Completely destructs benchmark list

In/out variables:

-P_BList              (type struct Bench **)
                      (For explanation of struct Bench type:
                      describes all aspects of modes of a benchmark;
                      see [1] for more information)

                      All substructures plus list itself are free-d
                      and NULL initialized

*/
/****************************************************************
Freeing of the Benchmark list
*****************************************************************/
/* IMB_3.0: take care of empty BList */
    if (*P_BList != (struct Bench*)NULL) {
        int i;
        i = 0;

        while ((*P_BList)[i].name) {
            IMB_v_free((void**)&((*P_BList)[i++].name));
        }
        IMB_v_free((void**)P_BList);
    }
}

void IMB_print_blist(struct comm_info * c_info, struct Bench *BList) {
/*

                      Displays requested benchmark scenario on stdout

Input variables:

-c_info               (type struct comm_info *)
                      Collection of all base data for MPI;
                      see [1] for more information

-BList                (type struct Bench *)
                      (For explanation of struct Bench type:
                      describes all aspects of modes of a benchmark;
                      see [1] for more information)

*/
    int j, ninvalid;
    char*nn, *cmt;
    char** def_cases, **General_cmt;

    IMB_get_def_cases(&def_cases, &General_cmt);

    if (General_cmt[0] != NULL) {
        fprintf(unit, "# Remarks on the current Version:\n\n");
        j = 0;

        while ((nn = General_cmt[j++]))
            fprintf(unit, "# %s\n", nn);
    }

    j = 0;
    ninvalid = 0;

    while (BList[j].name) {
        if (BList[j].RUN_MODES[0].type == BTYPE_INVALID) {
            ninvalid++;

            if (ninvalid == 1)
                fprintf(unit, "\n# Attention, invalid benchmark name(s):\n");

            fprintf(unit, "# %s\n", BList[j].name);
            IMB_v_free((void**)&(BList[j].name));
            BList[j].name = IMB_str("");
        }
        j++;
    }

    /* IMB_3.0 */
    if (ninvalid > 0) {
        /* IMB 3.1 << */
        int i = 0;
        fprintf(unit, "\n# List of valid benchmarks:\n#\n");
        /* >> IMB 3.1  */
        while (def_cases[i]) {
            fprintf(unit, "# %s\n", def_cases[i++]);
        }
    }

    if (ninvalid < j) {

        fprintf(unit, "\n# List of Benchmarks to run:\n\n");

        j = 0;
        while ((nn = BList[j].name)) {
            if (BList[j].RUN_MODES[0].type != BTYPE_INVALID) {
                if (c_info->group_mode >= 0)
                    fprintf(unit, "# (Multi-)%s\n", nn);
                else
                    fprintf(unit, "# %s\n", nn);

                if (*(BList[j].bench_comments))
                    fprintf(unit, "#     Comments on this Benchmark:\n");

                while ((cmt = *(BList[j].bench_comments++)))
                    fprintf(unit, "#     %s\n", cmt);
            }
            j++;
        }
    }

}
