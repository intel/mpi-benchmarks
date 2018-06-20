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
    
 File: IMB.c 

 Implemented functions: 

 main;

 ***************************************************************************/

#include "mpi.h"

#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_prototypes.h"

extern int num_alloc, num_free;


/**********************************************************************/



int main(int argc, char **argv) {
/*



Input variables:

-argc                 (type int)
                      Number of command line arguments


-argv                 (type char **)
                      List of command line arguments



Return value          (type int)
                      0 always



*/
    struct comm_info C_INFO;   /* BASIC SETTINGS STRUCTURE    */
    struct Bench* BList;       /* List of Benchmarks          */

    struct Bench* Bmark;

#ifdef CHECK
    int NFAIL, NSUCCESS;
#endif

    char   *p;
    int    i, j, iter, imod, ierr;
    int    NP, NP_min;
    int    do_it;

    int    header;
    int    size;
    int    MAXMSG;
    int    x_sample, n_sample;
    int    ci_np; /* number of procs adjusted in case of ParallelTransferMsgRate benchmark */
    /* IMB 3.1 << */
    struct iter_schedule ITERATIONS;
    int    mem_ok;
    /* >> IMB 3.1  */
    MODES  BMODE;
    double time[MAX_TIME_ID];

    Type_Size unit_size;

#ifdef USE_MPI_INIT_THREAD

    IMB_chk_arg_level_of_threading(&argv, &argc);

    if ((ierr = MPI_Init_thread(&argc, &argv, mpi_thread_desired, &mpi_thread_environment)) != MPI_SUCCESS)
        IMB_err_hand(1, ierr);
    if (mpi_thread_environment != mpi_thread_desired) {
        fprintf(unit, "ERROR: MPI_Init_thread was not able to set up desired threading level\n");
        IMB_err_hand(1, ierr);
    }
#else
    if ((ierr = MPI_Init(&argc, &argv)) != MPI_SUCCESS)
        IMB_err_hand(1, ierr);
#endif /*USE_MPI_INIT_THREAD*/

    IMB_set_default(&C_INFO);

    IMB_init_pointers(&C_INFO);

    /* IMB 3.1 << */
    if (IMB_basic_input(&C_INFO, &BList, &ITERATIONS, &argc, &argv, &NP_min) < 0) {
    /* >> IMB 3.1  */
        /* IMB_3.0: help mode */
        if(C_INFO.w_rank == 0)
            IMB_help();

        MPI_Barrier(MPI_COMM_WORLD);
        IMB_free_all(&C_INFO, &BList, &ITERATIONS);
        MPI_Finalize();
        return 0;
    }

    /* IMB 3.1 << */
    IMB_show_selections(&C_INFO, BList, &argc, &argv);
    /* >> IMB 3.1  */

    /* LOOP OVER INDIVIDUAL BENCHMARKS */
    j = 0;
    while (p = BList[j].name) {
        Bmark = BList+j;

        ci_np = C_INFO.w_num_procs;
        if (Bmark->RUN_MODES[0].type == ParallelTransferMsgRate) {
            ci_np -= ci_np % 2;
            NP_min += NP_min % 2;
        }
        if (Bmark->RUN_MODES[0].type != BTYPE_INVALID) {
            NP = max(1, min(ci_np, NP_min));

            if (Bmark->RUN_MODES[0].type == SingleTransfer || Bmark->RUN_MODES[0].type == SingleElementTransfer) {
#ifndef MPIIO
                NP = min(2, ci_np);
#else
                NP = 1;
#endif
            }

            do_it = 1;
/* LOOP OVER PROCESS NUMBERS */
            while (do_it) {
                if (IMB_valid(&C_INFO, Bmark, NP)) {
                    if (IMB_init_communicator(&C_INFO, NP) != 0)
                        IMB_err_hand(0, -1);
#ifdef MPIIO
                    if (IMB_init_file(&C_INFO, Bmark, &ITERATIONS, NP) !=0)
                        IMB_err_hand(0, -1);
#endif

#ifdef RMA
                    /* Now when communicator/s is created, update scaling of reported timing values.
                     * Some of RMA benchmarks accesses memory of all other processes, so their
                     * bandwidth results should be update accordingly */
                    IMB_adjust_timings_scale(&C_INFO, Bmark);
#endif
                    /* MINIMAL OUTPUT IF UNIT IS GIVEN */
                    if (C_INFO.w_rank == 0)
                        if (unit != stdout)
                            printf("# Running %s; see file \"%s\" for results\n", p, OUTPUT_FILENAME);
                    header = 1;

#ifdef EXT
                    MPI_Type_size(C_INFO.red_data_type, &unit_size);
#else
                    if (Bmark->reduction || Bmark->RUN_MODES[0].type == SingleElementTransfer)
                        MPI_Type_size(C_INFO.red_data_type, &unit_size);
                    else
                        MPI_Type_size(C_INFO.s_data_type, &unit_size);
#endif

                    MAXMSG = (1 << C_INFO.max_msg_log) / unit_size * unit_size;

                    for (imod = 0; imod < Bmark->N_Modes; imod++) {
                        double sample_time  = MPI_Wtime();
                        int    time_limit[] = {0, 0};

                        BMODE = &Bmark->RUN_MODES[imod];

                        /* IMB 3.1 << */
                        // x_sample calc => IMB_init_buffers_iter
                        /* >> IMB 3.1  */

                        header = header | (imod > 0);

                        iter = 0;
                        size = 0;

                        Bmark->sample_failure = 0;
                        /* LOOP OVER MESSAGE LENGTHS */
                        while ( ((C_INFO.n_lens == 0 && size < MAXMSG ) || (C_INFO.n_lens > 0  && iter < C_INFO.n_lens)) &&
                                (Bmark->sample_failure != SAMPLE_FAILED_TIME_OUT) ) {
                            if (Bmark->RUN_MODES[0].type == SingleElementTransfer) {
                                /* just one size needs to be tested (the size of one element) */
                                MPI_Type_size(C_INFO.red_data_type, &size);
                            } else {
                                if (C_INFO.n_lens > 0)
                                    size = C_INFO.msglen[iter];
                                else {
                                    if (iter == 0)
                                        size = 0;
                                    else {
                                        if (iter == 1) {
                                            size = ((1 << C_INFO.min_msg_log) + unit_size - 1) / unit_size * unit_size;
#ifdef EXT
                                            size = min(size, asize);
#endif
                                        } else
                                            size = min(MAXMSG, size + size);
                                    }
                                }
                            }

                            if (size > MAXMSG) {
                                if (C_INFO.w_rank == 0)
                                    fprintf(unit,"Attention, msg size %d truncated to %d\n", size, MAXMSG);
                                size = MAXMSG;
                            }

                            size = (size + unit_size - 1) / unit_size * unit_size;

                            if (Bmark->RUN_MODES[0].type == Sync) {
                                size = MAXMSG;
                                iter = C_INFO.n_lens - 1;
                            }

                            /* IMB 3.1 << */
                            // put some initialization stuff into:
                            IMB_init_buffers_iter(&C_INFO, &ITERATIONS, Bmark, BMODE, iter, size);

                            if (!Bmark->sample_failure) {
                                time_limit[1] = 0;
                                if (C_INFO.rank >= 0)
                                    time_limit[1] = (MPI_Wtime() - sample_time < max(max(C_INFO.n_lens, C_INFO.max_msg_log - C_INFO.min_msg_log) - 1, 1) * ITERATIONS.secs) ? 0 : 1;
                            }

                            MPI_Allreduce(&time_limit[1], &time_limit[0], 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

                            if (time_limit[0])
                                Bmark->sample_failure = SAMPLE_FAILED_TIME_OUT;

                            if( !Bmark->sample_failure ) {
                                IMB_warm_up(Bmark, &C_INFO, size, &ITERATIONS, iter);
#ifdef MPI1
                                C_INFO.select_source = Bmark->select_source;
#endif
                                Bmark->Benchmark(&C_INFO, size, &ITERATIONS, BMODE, time);
                            }
                            /* >> IMB 3.1  */

                            /* Synchronization, in particular for idle processes
                            which have to wait in a well defined manner */
                            MPI_Barrier(MPI_COMM_WORLD);

                            /* IMB 3.1 << */
                            IMB_output(&C_INFO, Bmark, BMODE, header, size, &ITERATIONS, time);
                            /* >> IMB 3.1  */

                            IMB_close_transfer(&C_INFO, Bmark, size);

                            CHK_BRK;

                            header = 0;
                            if (Bmark->RUN_MODES[0].type == SingleElementTransfer)
                                break;
                            iter++;
                        } /*while ( (C_INFO.n_lens...*/

                        if (!Bmark->success && C_INFO.w_rank == 0)
                            fprintf(unit,"\n\n!!! Benchmark unsuccessful !!!\n\n");

                        CHK_BRK;
                    } /*for (imod=0*/

                    CHK_BRK;
                } /*if (IMB_valid(&C_INFO, Bmark, NP))*/

                /* CALCULATE THE NUMBER OF PROCESSES FOR NEXT STEP */
                if (NP >= ci_np)
                    do_it = 0;
                else
                    NP = min(NP + NP, ci_np);

#ifdef MPIIO
                if (Bmark->RUN_MODES[0].type == SingleTransfer) do_it = 0;
#endif

                CHK_BRK;
            } /*while (do_it)*/
        } /*if (Bmark->RUN_MODES[0].type != BTYPE_INVALID)*/

        CHK_BRK;
        j++;
    } /*while (p = BList[j].name)*/

#ifdef CHECK
    if (C_INFO.w_rank == 0) {
        j        = 0;
        NFAIL    = 0;
        NSUCCESS = 0;

        while (p = BList[j].name) {
            Bmark = BList + j;

            if (Bmark->RUN_MODES[0].type != BTYPE_INVALID) {
                Bmark = BList + j;
                if (!Bmark->success) NFAIL++;
                else                 NSUCCESS++;
            }
            j++;
        }

        if (NFAIL == 0 && NSUCCESS > 0)
            fprintf(unit,"\n\n!!!!  ALL BENCHMARKS SUCCESSFUL !!!! \n\n");
        else if (NSUCCESS > 0)
        {
            if(NFAIL == 1)
                fprintf(unit,"\n\n!!!!  %d  BENCHMARK FAILED     !!!! \n\n", NFAIL);
            else
                fprintf(unit,"\n\n!!!!  %d  BENCHMARKS FAILED     !!!! \n\n", NFAIL);

            j = 0;

            while (p = BList[j].name) {
                Bmark = BList + j;
                if (Bmark->RUN_MODES[0].type != BTYPE_INVALID) {
                    if (Bmark->success)
                        fprintf(unit, "%s    : Successful\n", p);
                    else
                        fprintf(unit,"%s    : FAILED !! \n",p);
                }
                j++;
            }
        } /*else if (NSUCCESS > 0)*/
    } /*if (C_INFO.w_rank == 0)*/
#endif /*#ifdef CHECK*/


    /* IMB 3.1 << */
    IMB_free_all(&C_INFO, &BList, &ITERATIONS);

#ifdef CHECK
    if num_alloc == num_free)
        ierr=0;
    else {
        fprintf(stderr, "pr %d: calls to IMB_v_alloc %d / IMB_v_free %d (doesn't seem ok, are unequal!)\n", C_INFO.w_rank,num_alloc,num_free);
        ierr = 1;
    }

    MPI_Allreduce(&ierr, &mem_ok, 1, MPI_INT,MPI_MAX, MPI_COMM_WORLD);

    if (C_INFO.w_rank == 0)
        if( mem_ok == 0 )
            fprintf(stderr, "# of calls to IMB_v_alloc / IMB_v_free match on all processes\n");

#endif /*#ifdef CHECK*/
    MPI_Barrier(MPI_COMM_WORLD);
    IMB_end_msg(&C_INFO);

    /* >> IMB 3.1  */
    MPI_Finalize();

    return 0;
} /* end of main*/
