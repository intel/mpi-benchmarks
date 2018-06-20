/*****************************************************************************
 *                                                                           *
 * Copyright 2003-2018 Intel Corporation.                                    *
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
    
 File: IMB_output.c 

 Implemented functions: 

 IMB_output;
 IMB_display_times;
 IMB_calculate_times;
 IMB_show_selections;
 IMB_show_procids;
 IMB_print_array;
 IMB_print_int_row;
 IMB_print_info;
 IMB_print_header;
 IMB_edit_format;
 IMB_make_line;

New in IMB_3.0:
 IMB_help;

 ***************************************************************************/

#include <string.h>
#include <float.h> // DBL_MAX

#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_prototypes.h"

enum output_format {
    /* print msg size, number of iterations, time and bandwidth */
    OUT_TIME_AND_BW,

    /* print msg size, number of iterations, bandwidth and msg rate */
    OUT_BW_AND_MSG_RATE,

    /* print msg size, number of iterations,
     * min, max and avrg times (among all ranks) and bandwidth */
    OUT_TIME_RANGE_AND_BW,

    /* print msg size, number of iterations
     * min, max and avrg times (among all ranks) */
    OUT_TIME_RANGE,

    /* print pure communication time, total time, computation time and
       the overlap of computation and communication (in %-s) */
    OUT_OVERLAP,

    /* It is used for operations where msg size is not relevant
     * (for instance Barrier, Ibarrier). The format may differ for
     * different benchmarks, and msg size is not printed. */
    OUT_SYNC
};


/*****************************************************************/



/* IMB 3.1 << */
/*
Introduce new ITERATIONS object
*/
void IMB_output(struct comm_info* c_info, struct Bench* Bmark, MODES BMODE,
                int header, int size, struct iter_schedule* ITERATIONS,
                double *time) {
/* >> IMB 3.1  */
/*

Input variables:

-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information
                      
-Bmark                (type struct Bench*)
                      (For explanation of struct Bench type:
                      describes all aspects of modes of a benchmark;
                      see [1] for more information)

                      The actual benchmark
                      
-BMODE                (type MODES)
                      The actual benchmark mode (if relevant; only MPI-2 case, see [1])
                      
-header               (type int)
                      1/0 for do/don't print table headers
                      
-size                 (type int)
                      Benchmark message size

-ITERATIONS           (type struct iter_schedule)
                      Benchmark repetition descr. object


-time                 (type double *)
                      Benchmark timing outcome
                      3 numbers (min/max/average)

*/
    double scaled_time[MAX_TIME_ID];

    int i, i_gr;
    int li_len;
    int out_format;

    const int DO_OUT = (c_info->w_rank == 0) ? 1 : 0;
    const int GROUP_OUT = (c_info->group_mode > 0) ? 1 : 0;

    ierr = 0;

    if (DO_OUT) {
        /* Fix IMB_1.0.1: NULL all_times before allocation */
        IMB_v_free((void**)&all_times);

        all_times = (double*)IMB_v_alloc(c_info->w_num_procs * Bmark->Ntimes * sizeof(double), "Output 1");
#ifdef CHECK
        if (!all_defect) {
            all_defect = (double*)IMB_v_alloc(c_info->w_num_procs * sizeof(double), "Output 1");
            for (i = 0; i < c_info->w_num_procs; i++)
                all_defect[i] = 0.;
        }
#endif   
    } /*if (DO_OUT)*/

    /* Scale the timings */
    for (i = 0; i < Bmark->Ntimes; i++)
        scaled_time[i] = time[i] * SCALE * Bmark->scale_time;

    /* collect all times  */
    ierr = MPI_Gather(scaled_time, Bmark->Ntimes, MPI_DOUBLE, all_times, Bmark->Ntimes, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_ERRHAND(ierr);

#ifdef CHECK      
    /* collect all defects */
    ierr = MPI_Gather(&defect, 1, MPI_DOUBLE, all_defect, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_ERRHAND(ierr);
#endif

    if (DO_OUT) {
        BTYPES type = Bmark->RUN_MODES[0].type;
        const int n_groups = GROUP_OUT ? c_info->n_groups : 1;

        if (Bmark->RUN_MODES[0].NONBLOCKING && type != Sync) {
            out_format = OUT_OVERLAP;
        } else if ((type == SingleTransfer && c_info->group_mode != 0) ||
                    type == MultPassiveTransfer ||
                    (type == SingleElementTransfer && c_info->group_mode != 0)) {
            out_format = OUT_TIME_AND_BW;
        } else if (type == ParallelTransfer || type == SingleTransfer || type == SingleElementTransfer) {
            out_format = OUT_TIME_RANGE_AND_BW;
        } else if (type == ParallelTransferMsgRate) {
            out_format = OUT_BW_AND_MSG_RATE;
        } else if (type == Collective) {
#ifdef MPIIO
            out_format = OUT_TIME_RANGE_AND_BW;
#else
            out_format = OUT_TIME_RANGE;
#endif
        } else {
            out_format = OUT_SYNC;
        }

        if (header) {
            IMB_print_header(out_format, Bmark, c_info, BMODE);
        }

        if (GROUP_OUT) {
            fprintf(unit, "\n");
        }

        for (i_gr = 0; i_gr < n_groups; i_gr++) {
            IMB_display_times(Bmark, all_times, c_info, i_gr, ITERATIONS->n_sample, size, out_format);
        }
    } /*if( DO_OUT )*/
}


/*****************************************************************/
void IMB_display_times(struct Bench* Bmark, double* tlist, struct comm_info* c_info,
                       int group, int n_sample, int size, int out_format) {
/*

Input variables:

-Bmark                (type struct Bench*)
                      (For explanation of struct Bench type:
                      describes all aspects of modes of a benchmark;
                      see [1] for more information)

                      The actual benchmark


-tlist                (type double*)
                      Benchmark timing outcome
                      3 numbers (min/max/average)


-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information


-group                (type int)
                      Index of group to be displayed (multi-case only)


-n_sample             (type int)
                      Benchmark repetition number


-size                 (type int)
                      Benchmark message size


-out_format           (type int)
                      Code for table formatting details

*/
    int i, offset = 0, peers;
    static double MEGA = 1.0 / 1e6;

    double throughput = 0.;
    double overlap = 0.;
    double t_pure = 0.;
    double t_ovrlp = 0.;
    double t_comp = 0.;
    double msgrate = 0;

    Timing timing[MAX_TIME_ID]; // min, max and avg
#ifdef CHECK
    double defect = 0.;
#endif
    memset(&timing, 0, MAX_TIME_ID * sizeof(timing[MIN]));


    if (c_info->g_sizes[group] <= 0) {
        return;
    }

#ifdef CHECK
    IMB_calculate_times(Bmark->Ntimes, c_info, group, tlist, timing, &defect);
#else
    IMB_calculate_times(Bmark->Ntimes, c_info, group, tlist, timing);
#endif

#ifdef NBC
    if (!strstr(Bmark->name, "_pure")) {
        const size_t rank_index = timing[MAX].offset[OVRLP];
        t_pure = tlist[rank_index + PURE];
        t_ovrlp = tlist[rank_index + OVRLP];
        t_comp = tlist[rank_index + COMP];
        overlap = 100. * max(0., min(1., (t_pure + t_comp - t_ovrlp) / max(t_pure, t_comp)));
    }

#elif defined RMA
    /* RMA benchmarks which test truly passive synchronisation presence */
    if (Bmark->RUN_MODES[0].NONBLOCKING) {
        /* Time when the target was inside MPI stack */
        t_pure = timing[MAX].times[PURE];

        /* Time when the target was calculating something outside the MPI stack
         * for a while and then entered the MPI stack */
        t_ovrlp = timing[MAX].times[OVRLP];
    }

#else // NBC || RMA
    if (Bmark->RUN_MODES[0].NONBLOCKING) {
        t_pure = timing[MAX].times[PURE];
        t_ovrlp = timing[MAX].times[OVRLP];
        t_comp = tCPU;
        overlap = 100.* max(0, min(1, (t_pure + t_comp - t_ovrlp) / min(t_pure, t_comp)));
    }
#endif // NBC || RMA

    if (timing[MAX].times[PURE] > 0.) {
        if (Bmark->RUN_MODES[0].type != ParallelTransferMsgRate)
            throughput = (Bmark->scale_bw * SCALE * MEGA) * size / timing[MAX].times[PURE];
#ifndef MPIIO
        else {
            peers = c_info->num_procs / 2;
            msgrate = (Bmark->scale_bw * SCALE * MAX_WIN_SIZE * peers) / timing[MAX].times[PURE];
            throughput = MEGA * msgrate * size;
        }
#endif
    }

    if (c_info->group_mode > 0) {
        IMB_edit_format(1, 0);
        sprintf(aux_string, format, group);
        offset = strlen(aux_string);
    }

    if (Bmark->sample_failure) {
        IMB_edit_format(1, 0);
        sprintf(aux_string + offset, format, size);
        offset = strlen(aux_string);

        switch (Bmark->sample_failure) {
            case SAMPLE_FAILED_MEMORY:
                sprintf(aux_string + offset,
                        " out-of-mem.; needed X=%8.3f GB; use flag \"-mem X\" or MAX_MEM_USAGE>=X (IMB_mem_info.h)",
                        (1000. * c_info->used_mem + 1.) / 1000.);
                break;

            case SAMPLE_FAILED_INT_OVERFLOW:
                sprintf(aux_string + offset, " int-overflow.; The production rank*size caused int overflow for given sample");
                break;
            case SAMPLE_FAILED_TIME_OUT:
                aux_string[offset] = '\0';
                fprintf(unit, "%s%s", aux_string, " time-out.; Time limit (secs_per_sample * msg_sizes_list_len) is over; use \"-time X\" or SECS_PER_SAMPLE=X (IMB_settings.h) to increase time limit.");
                aux_string[0] = '\0';
                break;
        } /*switch*/
    } else {
        switch (out_format) {
            case OUT_TIME_AND_BW:
                IMB_edit_format(2, 2);
                sprintf(aux_string + offset, format, size, n_sample, timing[MAX].times[PURE], throughput);
                break;
            case OUT_BW_AND_MSG_RATE:
                IMB_edit_format(2, 1);
                offset += sprintf(aux_string + offset, format, size, n_sample, throughput);
                sprintf(&(format[0]), "%%%d.0f", ow_format);
                sprintf(aux_string + offset, format, msgrate);
                break;
            case OUT_TIME_RANGE_AND_BW:
                IMB_edit_format(2, 4);
                sprintf(aux_string + offset, format, size, n_sample, timing[MIN].times[PURE], timing[MAX].times[PURE], timing[AVG].times[PURE], throughput);
                break;
            case OUT_TIME_RANGE:
                IMB_edit_format(2, 3);
                sprintf(aux_string + offset, format, size, n_sample, timing[MIN].times[PURE], timing[MAX].times[PURE], timing[AVG].times[PURE]);
                break;
            case OUT_SYNC:
#ifdef NBC
                if (Bmark->RUN_MODES[0].NONBLOCKING && !strstr(Bmark->name, "_pure")) {
                    IMB_edit_format(1, 4);
                    sprintf(aux_string + offset, format, n_sample, t_ovrlp, t_pure, t_comp, overlap);
                } else
#endif // NBC
                {
                    IMB_edit_format(1, 3);
                    sprintf(aux_string + offset, format, n_sample, timing[MIN].times[PURE], timing[MAX].times[PURE], timing[AVG].times[PURE]);
                }
                break;
            case OUT_OVERLAP:
#ifdef RMA
                IMB_edit_format(2, 2);
                sprintf(aux_string + offset, format, size, n_sample, t_pure, t_ovrlp);
#else            
                IMB_edit_format(2, 4);
                sprintf(aux_string + offset, format, size, n_sample, t_ovrlp, t_pure, t_comp, overlap);
#endif            
                break;
        }

#ifdef CHECK 
        if (out_format != OUT_SYNC  && strcmp(Bmark->name, "Window")) {
            IMB_edit_format(0, 1);
            offset = strlen(aux_string);
            sprintf(aux_string + offset, format, defect);

            if (defect > TOL) {
                Bmark->success = 0;
            }
        }
#endif

    } /*if( Bmark->sample_failure )*/

    fprintf(unit, "%s\n", aux_string);
    fflush(unit);
}

void IMB_calculate_times(int ntimes,
                         struct comm_info* c_info,
                         int group_id,
                         double* tlist,
                         Timing* timing
#ifdef CHECK
                         , double* defect
#endif
                         ) {
    int offset = 0;
    int nproc = 0;
    int rank = 0;
    int times_count = 0;
    int i = 0;
    Time_Id time_id = PURE;

    const int is_group_mode = c_info->group_mode > 0 ? 1 : 0;


    int ncount = is_group_mode
                 ? group_id
                 : c_info->n_groups;

#ifdef CHECK
    *defect = 0;
#endif


    for (i = 0; i < ncount; i++) {
        nproc += c_info->g_sizes[i];
    }

    ncount = is_group_mode
             ? c_info->g_sizes[group_id]
             : nproc;

    for (time_id = PURE; time_id < ntimes; time_id++) {
        times_count = 0;
        timing[MIN].times[time_id] = DBL_MAX;

        for (i = 0; i < ncount; i++) {
            rank = is_group_mode
                   ? (nproc + i) * ntimes
                   : c_info->g_ranks[i] * ntimes;

            offset = rank + time_id;
            if (tlist[offset] < 0.) {
                continue;
            }
            times_count++;

            if (tlist[offset] < timing[MIN].times[time_id]) {
                timing[MIN].times[time_id] = tlist[offset];
                timing[MIN].offset[time_id] = rank;
            }

            if ((tlist[offset] > timing[MAX].times[time_id])) {
                timing[MAX].times[time_id] = tlist[offset];
                timing[MAX].offset[time_id] = rank;
            }

            timing[AVG].times[time_id] += tlist[offset];
#ifdef CHECK
            {
                const size_t check_index = is_group_mode
                    ? c_info->g_ranks[nproc + i]
                    : c_info->g_ranks[i];
                *defect = max(*defect, all_defect[check_index]);
            }
#endif 
        }
        // fixed 'times_count may be 0' issue
        if (times_count != 0)
            timing[AVG].times[time_id] /= times_count;
        else
            timing[AVG].times[time_id] = 0;
    }
}


/************************************************************************/
/* IMB 3.1 << */
// Re-display calling sequence
void IMB_show_selections(struct comm_info* c_info, struct Bench* BList, int *argc, char ***argv) {
/* >> IMB 3.1  */
/*

       Displays on stdout an overview of the user selections

Input variables:

-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information

-BList                (type struct Bench*)
                      (For explanation of struct Bench type:
                      describes all aspects of modes of a benchmark;
                      see [1] for more information)

                      The requested list of benchmarks

-argc                 (type int *)
                      Number of command line arguments

-argv                 (type char ***)
                      List of command line arguments

*/
    int iarg = 0;
    int i;

    if (c_info->w_rank == 0) {
        IMB_general_info();
        /* IMB 3.1 << */

        // repeat calling sequence
        fprintf(unit, "\n\n# Calling sequence was: \n\n#");

        while (iarg < *argc) {
            if (iarg > 0 && iarg % 6 == 0){
                if ((*argv)[iarg][0] == '-' && iarg + 1 < *argc){
                    fprintf(unit, " %s %s\n#", (*argv)[iarg], (*argv)[iarg + 1]);
                    iarg++;
                } else
                    fprintf(unit, " %s\n#", (*argv)[iarg]);

                for (i = 0; i <= strlen((*argv)[0]); i++)
                    fprintf(unit, " ");
            } else
                fprintf(unit, " %s", (*argv)[iarg]);

            iarg++;
        } /*while( iarg<*argc )*/

        fprintf(unit, "\n\n");
#ifndef MPIIO
        if (c_info->n_lens > 0) {
            fprintf(unit, "# Message lengths were user defined\n");
        } else {
            /* >> IMB 3.1  */
            fprintf(unit, "# Minimum message length in bytes:   %d\n", 0);
            fprintf(unit, "# Maximum message length in bytes:   %d\n", 1 << c_info->max_msg_log);
        }

        fprintf(unit, "#\n");
        fprintf(unit, "# MPI_Datatype                   :   MPI_BYTE \n");
        fprintf(unit, "# MPI_Datatype for reductions    :   MPI_FLOAT\n");
        fprintf(unit, "# MPI_Op                         :   MPI_SUM  \n");
#else
        if (c_info->n_lens > 0) {
            fprintf(unit, "# IO lengths were user defined\n");
        } else {
            fprintf(unit, "# Minimum io portion in bytes:   %d\n", 0);
            fprintf(unit, "# Maximum io portion in bytes:   %d\n", 1 << c_info->max_msg_log);
        }
        fprintf(unit, "#\n");
        IMB_print_info();
#endif
        fprintf(unit, "#\n");

#ifdef IMB_OPTIONAL
        fprintf(unit, "#\n\n");
        fprintf(unit, "# !! Attention: results have been achieved in\n");
        fprintf(unit, "# !! IMB_OPTIONAL mode.\n");
        fprintf(unit, "# !! Results may differ from standard case.\n");
        fprintf(unit, "#\n");
#endif

        fprintf(unit, "#\n");

        IMB_print_blist(c_info, BList);

        if (do_nonblocking) {
            fprintf(unit, "\n\n# For nonblocking benchmarks:\n\n");
            fprintf(unit, "# Function CPU_Exploit obtains an undisturbed\n");
            fprintf(unit, "# performance of %7.2f MFlops\n", MFlops);
        }

    } /*if(c_info->w_rank == 0 )*/

} /* end of IMB_show_selections*/

/****************************************************************************/
void IMB_show_procids(struct comm_info* c_info) {
    /*

       Prints to stdout the process ids (of group eventually)

       Input variables:

       -c_info               (type struct comm_info*)
       Collection of all base data for MPI;
       see [1] for more information



       */
    int ip, py, i, k, idle;

    if (c_info->w_rank == 0) {
        if (c_info->n_groups == 1) {
            if (c_info->px > 1 && c_info->py > 1) {
                fprintf(unit, "\n# #processes = %d; rank order (rowwise): \n",
                              c_info->num_procs);
                ip = 0;

                for (i = 0; i < c_info->px && ip < c_info->NP; i++) {
                    py = c_info->w_num_procs / c_info->px;
                    if (i < c_info->w_num_procs%c_info->px) py++;
                    py = min(py, c_info->NP - ip);
                    IMB_print_array(c_info->g_ranks + ip, 1, 0, py, "", unit);
                    fprintf(unit, "\n");
                    ip = ip + py;
                }
            } else
                fprintf(unit, "\n# #processes = %d \n", c_info->num_procs);

            idle = c_info->w_num_procs - c_info->num_procs;
        } /*if(c_info->n_groups == 1)*/

        if (c_info->n_groups != 1) {
            fprintf(unit, "\n# ( %d groups of %d processes each running simultaneous ) \n",
                          c_info->n_groups, c_info->num_procs);

            IMB_print_array(c_info->g_ranks, c_info->n_groups, 0,
                            c_info->g_sizes[0], "Group ", unit);

            idle = c_info->w_num_procs - c_info->n_groups*c_info->g_sizes[0];
        }

        if (idle) {
            if (idle == 1)
                fprintf(unit, "# ( %d additional process waiting in MPI_Barrier)\n", idle);
            else
                fprintf(unit, "# ( %d additional processes waiting in MPI_Barrier)\n", idle);
        }
    } /*if( c_info->w_rank == 0 )*/

} /* end of IMB_show_procids*/


/****************************************************************************/
void IMB_print_array(int* Array, int N, int disp_N,
                     int M, char* txt, FILE* unit) {
/*

       Formattedly prints to stdout a M by N int array

Input variables:

-Array                (type int*)
                      Array to be printed


-N                    (type int)
                      Number of rows to be printed

-disp_N               (type int)
                      Displacement in Array where frist row begins

-M                    (type int)
                      Number of columns

-txt                  (type char*)
                      Accompanying text

-unit                 (type FILE*)
                      Output unit

*/
#define MAX_SHOW 1024
    int i, j;

    char* outtxt;
    int do_out;

    do_out = 0;

    if (txt)
        if (strcmp(txt, "")) {
            outtxt = (char*)IMB_v_alloc((strlen(txt) + 6)*sizeof(char), " IMB_print_array ");
            do_out = 1;
        }

    if (N <= 1) {
        if (M > MAX_SHOW) {
            fprintf(unit, "#  ");
            IMB_print_int_row(unit, Array, MAX_SHOW / 2);
            fprintf(unit, " ... ");
            IMB_print_int_row(unit, &Array[M - MAX_SHOW / 2], MAX_SHOW / 2);
        } else {
            if (do_out) fprintf(unit, "# %s", txt);
            else         fprintf(unit, "# ");
            IMB_print_int_row(unit, Array, M);
        }
    } else if (N <= MAX_SHOW) {
        int zero = 0, one = 1;
        for (i = 0; i < N; i++) {
            if (do_out)
                sprintf(outtxt, "%s %d: ", txt, disp_N + i);
            else    outtxt = (char*)NULL;
            IMB_print_array(&Array[i*M], one, zero, M, outtxt, unit);

            fprintf(unit, "\n");
        }
    } else { /*for( i=0...*/
        int disp;

        disp = 0;
        IMB_print_array(Array, MAX_SHOW / 2, disp, M, txt, unit);
        fprintf(unit, "#  . \n");
        fprintf(unit, "#  . \n");
        disp = N - MAX_SHOW / 2;
        IMB_print_array(&Array[(N - MAX_SHOW / 2)*M], MAX_SHOW / 2, disp, M, txt, unit);
    }

    if (do_out) {
        IMB_v_free((void**)&outtxt);
    }
}

/****************************************************************************/
void IMB_print_int_row(FILE* unit, int* Array, int M) {
/*

       Formattedly prints to stdout a row of int numbers

Input variables:

-unit                 (type FILE*)
                      Output unit

-Array                (type int*)
                      Data to be printed

-M                    (type int)
                      Number of data

*/
#define X_PER_ROW 16
    int i, j, i0, irest;

    irest = M%X_PER_ROW;
    for (j = 0; j < (M + X_PER_ROW - 1) / X_PER_ROW; j++) {
        i0 = j*X_PER_ROW;

        for (i = 0; i < min(M - i0, X_PER_ROW); i++)
            fprintf(unit, " %4d", Array[i0 + i]);
        fprintf(unit, "\n# ");
    }
}

#ifdef MPIIO
/****************************************************************************/
void IMB_print_info() {
/*

       Prints MPI_Info selections (MPI-2 only)

*/
    int nkeys, ikey, vlen, exists;
    MPI_Info tmp_info;
    char key[MPI_MAX_INFO_KEY], *value;

    IMB_user_set_info(&tmp_info);

    /* July 2002 fix V2.2.1: handle NULL case */
    if (tmp_info != MPI_INFO_NULL) {
        /* end change */

        MPI_Info_get_nkeys(tmp_info, &nkeys);

        if (nkeys > 0) fprintf(unit, "# Got %d Info-keys:\n\n", nkeys);

        for (ikey = 0; ikey < nkeys; ikey++) {
            MPI_Info_get_nthkey(tmp_info, ikey, key);

            MPI_Info_get_valuelen(tmp_info, key, &vlen, &exists);

            value = (char*)IMB_v_alloc((vlen + 1)* sizeof(char), "Print_Info");

            MPI_Info_get(tmp_info, key, vlen, value, &exists);
            printf("# %s = \"%s\"\n", key, value);

            IMB_v_free((void**)&value);
        }

        MPI_Info_free(&tmp_info);

        /* July 2002 fix V2.2.1: end if */
    }
    /* end change */
}
#endif



/*****************************************************************/
void IMB_print_header(int out_format, struct Bench* bmark,
                      struct comm_info* c_info, MODES bench_mode) {

    int line_len = 0;
    char* help;
    char* token;

    fprintf(unit, "\n");            /* FOR GNUPLOT: CURVE SEPERATOR  */

    if (c_info->group_mode > 0) {
        /* several groups output*/
        strcpy(aux_string, "&Group");
        line_len = 1;
    } else {
        strcpy(aux_string, "");
        line_len = 0;
    }


    switch (out_format) {
        case OUT_TIME_AND_BW:
            line_len += 4;
            strcat(aux_string, "&#bytes&#repetitions&t[usec]&Mbytes/sec&");
            break;

        case OUT_BW_AND_MSG_RATE:
            line_len += 4;
            strcat(aux_string, "&#bytes&#repetitions&Mbytes/sec&Msg/sec&");
            break;

        case OUT_TIME_RANGE_AND_BW:
            line_len += 6;
            strcat(aux_string,
                   "&#bytes&#repetitions&t_min[usec]&t_max[usec]&t_avg[usec]&Mbytes/sec&");
            break;

        case OUT_TIME_RANGE:
            line_len += 5;
            strcat(aux_string, "&#bytes&#repetitions&t_min[usec]&t_max[usec]&t_avg[usec]&");
            break;

        case OUT_SYNC:
            if (bmark->RUN_MODES[0].NONBLOCKING && !strstr(bmark->name, "_pure")) {
                line_len += 5;
                strcat(aux_string,
                       "&#repetitions&t_ovrl[usec]&t_pure[usec]&t_CPU[usec]& overlap[%]&");
            } else {
                line_len += 4;
                strcat(aux_string,
                       "&#repetitions&t_min[usec]&t_max[usec]&t_avg[usec]&");
            }
            break;

        case OUT_OVERLAP:
#ifdef RMA
            line_len += 4;
            strcat(aux_string,
                   "&#bytes&#repetitions&t_pure[usec]&t_ovrl[usec]&");
#else        
            line_len += 6;
            strcat(aux_string,
                "&#bytes&#repetitions&t_ovrl[usec]&t_pure[usec]&t_CPU[usec]& overlap[%]&");
#endif        
            break;
    }

#ifdef CHECK
    if (bmark->RUN_MODES[0].type != Sync && strcmp(bmark->name, "Window")) {
        line_len += 1;
        strcat(aux_string, "&defects&");
    }
#endif

    IMB_make_line(line_len);

    if (c_info->n_groups > 1) {
        fprintf(unit, "# Benchmarking Multi-%s ", bmark->name);
    } else {
        fprintf(unit, "# Benchmarking %s ", bmark->name);
    }

    IMB_show_procids(c_info);

    IMB_make_line(line_len);

    switch (bench_mode->AGGREGATE) {
        case 1:
            fprintf(unit, "#\n#    MODE: AGGREGATE \n#\n");
            break;

        case 0:
            fprintf(unit, "#\n#    MODE: NON-AGGREGATE \n#\n");
            break;
    }

    help = aux_string;
    while (token = strtok(help, "&")) {
        sprintf(format, "%%%ds", ow_format);
        fprintf(unit, format, token);
        help = NULL;
    }
    fprintf(unit, "\n");

    return;
}


/*****************************************************************/
void IMB_edit_format(int n_ints, int n_floats) {
/*

       Edits format string for output

       In/out variables:

       -n_ints               (type int)
# of int items to be printed


        -n_floats             (type int)
# of float items to be printed

*/
    int ip, i;

    ip = 0;

    for (i = 1; i <= n_ints; i++) {
        sprintf(&(format[ip]), "%%%dd", ow_format);
        ip = strlen(format);
    }

    for (i = 1; i <= n_floats; i++) {
        sprintf(&(format[ip]), "%%%d.2f", ow_format);
        ip = strlen(format);
    }
}

/***************************************************************************/
void IMB_make_line(int line_len) {
/*

       Prints an underline

Input variables:

-line_len               (type int)
                        Length of underline

*/
    int i;
    char* char_line = "-";
    fprintf(unit, "#");

    for (i = 1; i < line_len*ow_format; i++)
        fprintf(unit, "%s", char_line);
    fprintf(unit, "\n");
}

/***************************************************************************/
/* New function for IMB_3.0 */
void IMB_help()
{

#ifdef MPI1
    const char *progname = "IMB-MPI1";
#elif defined(EXT)
    const char *progname = "IMB-EXT";
#elif defined (MPIIO)
    const char *progname = "IMB-IO";
#elif defined (RMA)
    const char *progname = "IMB-RMA";
#elif defined (NBC)
    const char *progname = "IMB-NBC";
#endif

    fflush(stderr);
    fflush(unit);

    fprintf(unit, "\nCalling sequence (command line will be repeated in Output table!):\n\n");

    fprintf(unit, "\n%s         [-h{elp}]\n", progname);

    /* IMB 3.1 << */
    /* Update calling sequence */
    /* >> IMB 3.1  */
    fprintf(unit,
        "[-npmin        <NPmin>]\n"
        "[-multi        <outflag>]\n"
        "[-off_cache    <cache_size[,cache_line_size]>\n"
        "[-iter         <msgspersample[,overall_vol[,msgs_nonaggr]]>\n"
        "[-iter_policy  <iter_policy>]\n"
        "[-time         <max_runtime per sample>]\n"
        "[-mem          <max. per process memory for overall message buffers>]\n"
        "[-msglen       <Lengths_file>]\n"
        "[-map          <PxQ>]\n"
        "[-input        <filename>]\n"
        "[benchmark1    [benchmark2 [...]]]\n"
        "[-include      [benchmark1,[benchmark2,[...]]]\n"
        "[-exclude      [benchmark1,[benchmark2,[...]]]\n"
        "[-msglog       <[min_msglog]:max_msglog>]\n"
#if (defined MPI1 || defined NBC)
        "[-root_shift   <on or off>]\n"
        "[-sync         <on or off>]\n"
#endif            
        "[-imb_barrier  <on or off>]\n"
        "\n"
        "where \n"
        "\n"
        "- h ( or help) just provides basic help \n"
        "(if active, all other arguments are ignored)\n"
        "\n"
        "- npmin\n\n"
        "the argumaent after npmin is NPmin, \n"
        "the minimum number of processes to run on\n"
        "(then if IMB is started on NP processes, the process numbers \n"
        "NPmin, 2*NPmin, ... ,2^k * NPmin < NP, NP are used)\n"
        ">>>\n"
        "to run on just NP processes, run IMB on NP and select -npmin NP\n"
        "<<<\n"
        "default: \n"
        "NPmin=2\n"
        "\n");
#ifdef MPIIO
    fprintf(unit,
        "-off_cache \n\n"
        "no effect for IMB-IO (only IMB-MPI1, IMB-EXT) \n"
        "\n");
#else
    fprintf(unit,
        "- off_cache \n"
        "\n"
        "the argument after off_cache can be either 1 single number (cache_size),  \n"
        "or 2 comma separated numbers (cache_size,cache_line_size), or just -1 \n"
        "\n"
        "By default, without this flag, the communications buffer is  \n"
        "the same within all repetitions of one message size sample;   \n"
        "most likely, cache reusage is yielded and thus throughput results   \n"
        "that might be non realistic.    \n"
        "\n"
        "With -off_cache, it is attempted to avoid cache reusage.    \n"
        "cache_size is a float for an upper bound of the size of the last level cache in MBytes \n"
        "cache_line_size is assumed to be the size (Bytes) of a last level cache line  \n"
        "(can be an upper estimate).  \n"
        "The sent/recv'd data are stored in buffers of size ~ 2 x MAX( cache_size, message_size );  \n"
        "when repetitively using messages of a particular size, their addresses are advanced within those  \n"
        "buffers so that a single message is at least 2 cache lines after the end of the previous message.  \n"
        "Only when those buffers have been marched through (eventually), they will re-used from the beginning.  \n"
        "\n"
        "A cache_size and a cache_line_size are assumed as statically defined    \n"
        "in  => IMB_mem_info.h; these are used when -off_cache -1 is entered  \n"
        "\n"
        "remark: -off_cache is effective for IMB-MPI1, IMB-EXT, but not IMB-IO  \n"
        "\n"
        "examples  \n"
        "-off_cache -1 (use defaults of IMB_mem_info.h);  \n"
        "-off_cache 2.5 (2.5 MB last level cache, default line size);  \n"
        "-off_cache 16,128 (16 MB last level cache, line size 128);  \n"
        "\n"
        "NOTE: the off_cache mode might also be influenced by eventual internal  \n"
        "caching with the MPI library. This could make the interpretation \n"
        "intricate.  \n"
        "\n"
        "default: \n"
        "no cache control, data likely to come out of cache most of the time \n"
        "\n");
#endif
    fprintf(unit,
        "- iter \n\n"
        "the argument after -iter can contain from 1 to 3 comma separated values\n"
        "3 integer numbers override the defaults \n"
        "MSGSPERSAMPLE, OVERALL_VOL, MSGS_NONAGGR of =>IMB_settings.h\n"
        "examples \n"
        "-iter 2000        (override MSGSPERSAMPLE by value 2000) \n"
        "-iter 1000,100    (override OVERALL_VOL by 100) \n"
        "-iter 1000,40,150 (override MSGS_NONAGGR by 150) \n"
        "\n"
        "\n"
        "default: \n"
        "iteration control through parameters MSGSPERSAMPLE,OVERALL_VOL,MSGS_NONAGGR => IMB_settings.h \n"
        "\n"
        "- iter_policy\n"
        "\n"
        "the argument after -iter_policy is a one from possible strings,\n"
        "specifying that policy will be used for auto iteration control:\n"
        "dynamic,multiple_np,auto,off\n"
        "example \n"
        "-iter_policy auto\n"
        "default:\n"
        "iteration control through parameter ITER_POLICY => IMB_settings.h \n"
        "\n"
        "NOTE: !! New in versions from IMB 3.2 on !!  \n"
        "the iter selection is overridden by a dynamic selection that is a new default in \n"
        "IMB 3.2: when a maximum run time (per sample) is expected to be exceeded, the \n"
        "iteration number will be cut down; see -time flag  \n"
        "\n"
        "- time\n"
        "\n"
        "the argument after -time is a float, specifying that \n"
        "a benchmark will run at most that many seconds per message size \n"
        "the combination with the -iter flag or its defaults is so that always \n"
        "the maximum number of repetitions is chosen that fulfills all restrictions \n"
        "example \n"
        "-time 0.150       (a benchmark will (roughly) run at most 150 milli seconds per message size, iff\n"
        "the default (or -iter selected) number of repetitions would take longer than that) \n"
        "\n"
        "remark: per sample, the rough number of repetitions to fulfill the -time request \n"
        "is estimated in preparatory runs that use ~ 1 second overhead \n"
        "\n"
        "default: \n"
        "A fixed time limit SECS_PER_SAMPLE =>IMB_settings.h; currently set to 10  \n"
        "(new default in IMB_3.2) \n"
        "\n"
        "- mem\n\n"
        "the argument after -mem is a float, specifying that \n"
        "at most that many GBytes are allocated per process for the message buffers \n"
        "if the size is exceeded, a warning will be output, stating how much memory \n"
        "would have been necessary, but the overall run is not interrupted \n"
        "example \n"
        "-mem 0.2         (restrict memory for message buffers to 200 MBytes per process) \n"
        "\n"
        "default: \n"
        "the memory is restricted by MAX_MEM_USAGE => IMB_mem_info.h \n"
        "\n"
        "- map\n\n"
        "the argument after -map is PxQ, P,Q are integer numbers with P*Q <= NP\n"
        "enter PxQ with the 2 numbers separated by letter \"x\" and no blancs\n"
        "the basic communicator is set up as P by Q process grid\n"
        "\n"
        "if, e.g., one runs on N nodes of X processors each, and inserts\n"
        "P=X, Q=N, then the numbering of processes is \"inter node first\"\n"
        "running PingPong with P=X, Q=2 would measure inter-node performance\n"
        "(assuming MPI default would apply 'normal' mapping, i.e. fill nodes\n"
        "first priority) \n"
        "\n"
        "default: \n"
        "Q=1\n"
        "\n"
        "- multi\n\n"
        "the argument after -multi is outflag (0 or 1)\n"
        "\n"
        "if -multi is selected, running the N process version of a benchmark\n"
        "on NP overall, means running on (NP/N) simultaneous groups of N each.\n"
        "\n"
        "outflag only controls default (0) or extensive (1) output charts.\n"
        "0: only lowest performance groups is output\n"
        "1: all groups are output\n"
        "\n"
        "default: \n"
        "multi off\n"
        "\n"
        "- msglen\n\n"
        "the argument after -msglen is a lengths_file, an ASCII file, containing any set of nonnegative\n"
        "message lengths, 1 per line\n"
        "\n"
        "default: \n"
        "no lengths_file, lengths defined by settings.h, settings_io.h\n"
        "\n"
        "- input\n\n"
        "the argument after -input is a filename is any text file containing, line by line, benchmark names\n"
        "facilitates running particular benchmarks as compared to using the\n"
        "command line.\n"
        "default: \n"
        "no input file exists\n"
        "\n"
        "- include\n\n"
        "the argument after -include  is one or more benchmark names separated by comma\n"
        "\n"
        "- exclude\n\n"
        "the argument after -exclude  is one or more benchmark names separated by comma\n"
        "\n"
        "\n"
        "-msglog\n\n"
        "the argument after -msglog min:max, min and max are positive integer numbers, min<max\n"
        "where min is power of 2 so that second smallest data transfer size is max(unit,2^min)\n"
        "(the smallest always being 0), where unit = sizeof(float) for reductions, unit = 1 else\n"
        "max is power of 2 so that 2^max is largest messages size, max must be less than 31"
        "\n\n"
#if (defined MPI1 || defined NBC)
        "-root_shift\n\n"
        "controls root change at each iteration step for certain collective benchmarks,\n"
        "possible argument values are on (1|enable|yes) or off (0|disable|no)\n"
        "default:\n"
        "off\n"
        "\n"
        "-sync\n\n"
        "controls whether all processes are syncronized at each iteration step in collective benchmarks,\n"
        "possible argument values are on (1|enable|yes) or off (0|disable|no)\n"
        "default:\n"
        "on\n"
        "\n"
        "\n"
#endif            
        "-imb_barrier\n\n"
        "use internal MPI-independent barrier syncronization implementation,\n"
        "possible argument values are on (1|enable|yes) or off (0|disable|no)\n"
        "default:\n"
        "off\n"
        "\n"
        "- benchmarkX is (in arbitrary lower/upper case spelling)\n"
        "\n");
#ifdef MPI1
    fprintf(unit,
        "PingPongSpecificSource\n"
        "PingPongAnySource\n"
        "PingPingSpecificSource\n"
        "PingPingAnySource\n"
        "PingPing\n"
        "PingPong\n"
        "Sendrecv\n"
        "Exchange\n"
        "Bcast\n"
        "Allgather\n"
        "Allgatherv\n"
        "Gather\n"
        "Gatherv\n"
        "Scatter\n"
        "Scatterv\n"
        "Alltoall\n"
        "Alltoallv\n"
        "Reduce\n"
        "Reduce_scatter\n"
        "Allreduce\n"
        "Barrier\n"
        "Uniband\n"
        "Biband\n"
        "\n");

#elif defined(EXT)
    fprintf(unit,
        "Window\n"
        "Unidir_Put\n"
        "Unidir_Get\n"
        "Bidir_Get\n"
        "Bidir_Put\n"
        "Accumulate\n"
        "\n");

#elif defined(MPIIO)
    fprintf(unit,
        "S_Write_indv\n"
        "S_Read_indv\n"
        "S_Write_expl\n"
        "S_Read_expl\n"
        "P_Write_indv\n"
        "P_Read_indv\n"
        "P_Write_expl\n"
        "P_Read_expl\n"
        "P_Write_shared\n"
        "P_Read_shared\n"
        "P_Write_priv\n"
        "P_Read_priv\n"
        "C_Write_indv\n"
        "C_Read_indv\n"
        "C_Write_expl\n"
        "C_Read_expl\n"
        "C_Write_shared\n"
        "C_Read_shared\n"
        "\n");

#elif defined(NBC) 
    fprintf(unit,
        "Ibcast\n"
        "Ibcast_pure\n"
        "Iallgather\n"
        "Iallgather_pure\n"
        "Iallgatherv\n"
        "Iallgatherv_pure\n"
        "Igather\n"
        "Igather_pure\n"
        "Igatherv\n"
        "Igatherv_pure\n"
        "Iscatter\n"
        "Iscatter_pure\n"
        "Iscatterv\n"
        "Iscatterv_pure\n"
        "Ialltoall\n"
        "Ialltoall_pure\n"
        "Ialltoallv\n"
        "Ialltoallv_pure\n"
        "Ireduce\n"
        "Ireduce_pure\n"
        "Ireduce_scatter\n"
        "Ireduce_scatter_pure\n"
        "Iallreduce\n"
        "Iallreduce_pure\n"
        "Ibarrier\n"
        "Ibarrier_pure\n"
        "\n");

#elif defined(RMA)
    fprintf(unit,
        "Unidir_put\n"
        "Unidir_get\n"
        "Bidir_put\n"
        "Bidir_get\n"
        "One_put_all\n"
        "One_get_all\n"
        "All_put_all\n"
        "All_get_all\n"
        "Put_local\n"
        "Get_local\n"
        "Put_all_local\n"
        "Get_all_local\n"
        "Exchange_put\n"
        "Exchange_get\n"
        "Accumulate\n"
        "Get_accumulate\n"
        "Fetch_and_op\n"
        "Compare_and_swap\n"
        "Truly_passive_put\n"
        "\n");
#endif

}
