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

***************************************************************************/
#ifndef IMB_PROTOTYPES_H
#define IMB_PROTOTYPES_H

#include "IMB_benchmark.h"

/* MPI-1 message passing benchmarks */

#ifdef MPI1

void IMB_allgather(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                   MODES RUN_MODE, double* time);

void IMB_allgatherv(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                    MODES RUN_MODE, double* time);

void IMB_gather(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                MODES RUN_MODE, double* time);

void IMB_gatherv(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                 MODES RUN_MODE, double* time);

void IMB_scatter(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                 MODES RUN_MODE, double* time);

void IMB_scatterv(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                  MODES RUN_MODE, double* time);

void IMB_allreduce(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                   MODES RUN_MODE, double* time);

void IMB_alltoall(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                  MODES RUN_MODE, double* time);

void IMB_alltoallv(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                   MODES RUN_MODE, double* time);

void IMB_barrier(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                 MODES RUN_MODE, double* time);

void IMB_bcast(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
               MODES RUN_MODE, double* time);

void IMB_exchange(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                  MODES RUN_MODE, double* time);

void IMB_pingping(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                  MODES RUN_MODE, double* time);

void IMB_pingpong(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                  MODES RUN_MODE, double* time);

void IMB_reduce(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                MODES RUN_MODE, double* time);

void IMB_reduce_local(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                      MODES RUN_MODE, double* time);

void IMB_reduce_scatter(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                        MODES RUN_MODE, double* time);

void IMB_reduce_scatter_block(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                              MODES RUN_MODE, double* time);

void IMB_sendrecv(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                  MODES RUN_MODE, double* time);

void IMB_uni_bandwidth(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                       MODES RUN_MODE, double* time);

void IMB_bi_bandwidth(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                      MODES RUN_MODE, double* time);
#endif // MPI1

/* MPI-2 onesided communications benchmarks */

#ifdef EXT

void IMB_accumulate(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                    MODES RUN_MODE, double* time);

void IMB_bidir_get(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                   MODES RUN_MODE, double* time);

void IMB_bidir_put(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                   MODES RUN_MODE, double* time);

void IMB_unidir_put(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                    MODES RUN_MODE, double* time);

void IMB_unidir_get(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                    MODES RUN_MODE, double* time);

void IMB_ones_get(struct comm_info* c_info, int s_num, int dest,
                  int r_num, int sender, int size,
                  struct iter_schedule* ITERATIONS, double* time);

void IMB_ones_mget(struct comm_info* c_info, int s_num, int dest,
                   int r_num, int sender, int size,
                   struct iter_schedule* ITERATIONS, double* time);

void IMB_ones_put(struct comm_info* c_info, int s_num, int dest,
                  int r_num, int sender, int size,
                  struct iter_schedule* ITERATIONS, double* time);

void IMB_ones_mput(struct comm_info* c_info, int s_num, int dest,
                   int r_num, int sender, int size,
                   struct iter_schedule* ITERATIONS, double* time);

void IMB_window(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                MODES RUN_MODE, double* time);

void IMB_user_set_info(MPI_Info* opt_info);

#endif // EXT

/* MPI-IO benchmarks */

#ifdef MPIIO

void IMB_open_close(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                    MODES RUN_MODE, double* time);

void IMB_read_shared(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                     MODES RUN_MODE, double* time);

void IMB_read_indv(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                   MODES RUN_MODE, double* time);

void IMB_read_expl(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                   MODES RUN_MODE, double* time);

void IMB_read_ij(struct comm_info* c_info, int size, POSITIONING pos,
                 BTYPES type, int i_sample, int j_sample,
                 int time_inner, double* time);

void IMB_iread_ij(struct comm_info* c_info, int size, POSITIONING pos,
                  BTYPES type, int i_sample, int j_sample,
                  int time_inner, int do_ovrlp, double* time);

void IMB_write_shared(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                      MODES RUN_MODE, double* time);

void IMB_write_indv(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                    MODES RUN_MODE, double* time);

void IMB_write_expl(struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS,
                    MODES RUN_MODE, double* time);

void IMB_write_ij(struct comm_info* c_info, int size, POSITIONING pos,
                  BTYPES type, int i_sample, int j_sample,
                  int time_inner, double* time);

void IMB_iwrite_ij(struct comm_info* c_info, int size, POSITIONING pos,
                   BTYPES type, int i_sample, int j_sample,
                   int time_inner, int do_ovrlp, double* time);

void IMB_user_set_info(MPI_Info* opt_info);

#endif // MPIIO

#ifdef NBC

void IMB_ibcast(struct comm_info* c_info,
                int size,
                struct iter_schedule* ITERATIONS,
                MODES RUN_MODE,
                double* time);

void IMB_ibcast_pure(struct comm_info* c_info,
                     int size,
                     struct iter_schedule* ITERATIONS,
                     MODES RUN_MODE,
                     double* time);

void IMB_iallgather(struct comm_info* c_info,
                    int size,
                    struct iter_schedule* ITERATIONS,
                    MODES RUN_MODE,
                    double* time);

void IMB_iallgather_pure(struct comm_info* c_info,
                         int size,
                         struct iter_schedule* ITERATIONS,
                         MODES RUN_MODE,
                         double* time);

void IMB_iallgatherv(struct comm_info* c_info,
                     int size,
                     struct iter_schedule* ITERATIONS,
                     MODES RUN_MODE,
                     double* time);

void IMB_iallgatherv_pure(struct comm_info* c_info,
                          int size,
                          struct iter_schedule* ITERATIONS,
                          MODES RUN_MODE,
                          double* time);

void IMB_igather(struct comm_info* c_info,
                 int size,
                 struct iter_schedule* ITERATIONS,
                 MODES RUN_MODE,
                 double* time);

void IMB_igather_pure(struct comm_info* c_info,
                      int size,
                      struct iter_schedule* ITERATIONS,
                      MODES RUN_MODE,
                      double* time);

void IMB_igatherv(struct comm_info* c_info,
                  int size,
                  struct iter_schedule* ITERATIONS,
                  MODES RUN_MODE,
                  double* time);

void IMB_igatherv_pure(struct comm_info* c_info,
                       int size,
                       struct iter_schedule* ITERATIONS,
                       MODES RUN_MODE,
                       double* time);

void IMB_iscatter(struct comm_info* c_info,
                  int size,
                  struct iter_schedule* ITERATIONS,
                  MODES RUN_MODE,
                  double* time);

void IMB_iscatter_pure(struct comm_info* c_info,
                       int size,
                       struct iter_schedule* ITERATIONS,
                       MODES RUN_MODE,
                       double* time);

void IMB_iscatterv(struct comm_info* c_info,
                   int size,
                   struct iter_schedule* ITERATIONS,
                   MODES RUN_MODE,
                   double* time);

void IMB_iscatterv_pure(struct comm_info* c_info,
                        int size,
                        struct iter_schedule* ITERATIONS,
                        MODES RUN_MODE,
                        double* time);

void IMB_ialltoall(struct comm_info* c_info,
                   int size,
                   struct iter_schedule* ITERATIONS,
                   MODES RUN_MODE,
                   double* time);

void IMB_ialltoall_pure(struct comm_info* c_info,
                        int size,
                        struct iter_schedule* ITERATIONS,
                        MODES RUN_MODE,
                        double* time);

void IMB_ialltoallv(struct comm_info* c_info,
                    int size,
                    struct iter_schedule* ITERATIONS,
                    MODES RUN_MODE,
                    double* time);

void IMB_ialltoallv_pure(struct comm_info* c_info,
                         int size,
                         struct iter_schedule* ITERATIONS,
                         MODES RUN_MODE,
                         double* time);

void IMB_ireduce(struct comm_info* c_info,
                 int size,
                 struct iter_schedule* ITERATIONS,
                 MODES RUN_MODE,
                 double* time);

void IMB_ireduce_pure(struct comm_info* c_info,
                      int size,
                      struct iter_schedule* ITERATIONS,
                      MODES RUN_MODE,
                      double* time);

void IMB_ireduce_scatter(struct comm_info* c_info,
                         int size,
                         struct iter_schedule* ITERATIONS,
                         MODES RUN_MODE,
                         double* time);

void IMB_ireduce_scatter_pure(struct comm_info* c_info,
                              int size,
                              struct iter_schedule* ITERATIONS,
                              MODES RUN_MODE,
                              double* time);

void IMB_iallreduce(struct comm_info* c_info,
                    int size,
                    struct iter_schedule* ITERATIONS,
                    MODES RUN_MODE,
                    double* time);

void IMB_iallreduce_pure(struct comm_info* c_info,
                         int size,
                         struct iter_schedule* ITERATIONS,
                         MODES RUN_MODE,
                         double* time);

void IMB_ibarrier(struct comm_info* c_info,
                  int size,
                  struct iter_schedule* ITERATIONS,
                  MODES RUN_MODE,
                  double* time);

void IMB_ibarrier_pure(struct comm_info* c_info,
                       int size,
                       struct iter_schedule* ITERATIONS,
                       MODES RUN_MODE,
                       double* time);

#endif // NBC

#ifdef RMA

void IMB_rma_single_put(struct comm_info* c_info, int size,
                        struct iter_schedule* ITERATIONS,
                        MODES RUN_MODE, double* time);

void IMB_rma_single_get(struct comm_info* c_info, int size,
                        struct iter_schedule* ITERATIONS,
                        MODES RUN_MODE, double* time);

void IMB_rma_put_all(struct comm_info* c_info, int size,
                     struct iter_schedule* iterations,
                     MODES run_mode, double* time);

void IMB_rma_get_all(struct comm_info* c_info, int size,
                     struct iter_schedule* iterations,
                     MODES run_mode, double* time);

void IMB_rma_put_local(struct comm_info* c_info, int size,
                       struct iter_schedule* iterations,
                       MODES run_mode, double* time);

void IMB_rma_get_local(struct comm_info* c_info, int size,
                       struct iter_schedule* iterations,
                       MODES run_mode, double* time);

void IMB_rma_put_all_local(struct comm_info* c_info, int size,
                           struct iter_schedule* iterations,
                           MODES run_mode, double* time);

void IMB_rma_get_all_local(struct comm_info* c_info, int size,
                           struct iter_schedule* iterations,
                           MODES run_mode, double* time);

void IMB_rma_exchange_put(struct comm_info* c_info, int size,
                          struct iter_schedule* iterations,
                          MODES run_mode, double* time);

void IMB_rma_exchange_get(struct comm_info* c_info, int size,
                          struct iter_schedule* iterations,
                          MODES run_mode, double* time);

void IMB_rma_accumulate(struct comm_info* c_info, int size,
                        struct iter_schedule* iterations,
                        MODES run_mode, double* time);

void IMB_rma_get_accumulate(struct comm_info* c_info, int size,
                            struct iter_schedule* iterations,
                            MODES run_mode, double* time);

void IMB_rma_fetch_and_op(struct comm_info* c_info, int size,
                          struct iter_schedule* iterations,
                          MODES run_mode, double* time);

void IMB_rma_compare_and_swap(struct comm_info* c_info, int size,
                              struct iter_schedule* iterations,
                              MODES run_mode, double* time);

void IMB_rma_passive_put(struct comm_info* c_info, int size,
                         struct iter_schedule* iterations,
                         MODES run_mode, double* time);

void IMB_user_set_info(MPI_Info* opt_info);


#endif /*RMA*/



/* Organizational functions */


int main(int argc, char **argv);

/* IMB 3.1 << */
int IMB_basic_input(struct comm_info* c_info, struct Bench** P_BList,
                    struct iter_schedule* ITERATIONS,
                    int *argc, char ***argv, int* NP_min);

#ifdef USE_MPI_INIT_THREAD
void IMB_chk_arg_level_of_threading(char ***argv, int *argc);
#endif
/* >> IMB 3.1  */

void IMB_get_rank_portion(int rank, int NP, size_t size,
                          size_t unit_size, size_t* pos1, size_t* pos2);

int IMB_init_communicator(struct comm_info* c_info, int NP);

void IMB_adjust_timings_scale(struct comm_info *c_info, struct Bench *bmark);

void IMB_set_communicator(struct comm_info *c_info);

int IMB_valid(struct comm_info * c_info, struct Bench* Bmark, int NP);

void IMB_set_default(struct comm_info* c_info);

int  IMB_get_def_cases(char*** defc, char*** Gcmt);
int  IMB_get_all_cases(char*** allc);
void IMB_set_bmark(struct Bench* Bmark);

#ifdef MPIIO


void IMB_init_file_content(void* BUF, int pos1, int pos2);

/* << IMB 3.1 */
int IMB_init_file(struct comm_info* c_info, struct Bench* Bmark, struct iter_schedule* ITERATIONS, int NP);
/* >> IMB 3.1 */

void IMB_free_file(struct comm_info * c_info);

void IMB_del_file(struct comm_info* c_info);

void IMB_open_file(struct comm_info* c_info);


#endif /*MPIIO*/

void IMB_list_names(char* Bname, int** List);

int  IMB_get_bmark_index(char* name);

//void IMB_construct_blist(struct Bench** P_BList, int n_args, char* name);
void IMB_construct_blist(struct Bench* Bmark, const char* bname);

void IMB_construct_blist_default(struct Bench** P_BList);

void IMB_destruct_blist(struct Bench ** P_BList);

void IMB_print_blist(struct comm_info * c_info, struct Bench *BList);

void IMB_init_transfer(struct comm_info* c_info, struct Bench* Bmark, int size, MPI_Aint acc_size);

void IMB_close_transfer(struct comm_info* c_info, struct Bench* Bmark, int size);

void IMB_warm_up(struct Bench* Bmark, struct comm_info* c_info, int size, struct iter_schedule* ITERATIONS, int iter);

void IMB_cpu_exploit(float target_secs, int initialize);
double IMB_cpu_exploit_reworked(float target_secs, int initialize);

void IMB_general_info();

void IMB_make_sys_info();

void IMB_end_msg(struct comm_info* c_info);

/* IMB 3.1 << */
void IMB_output(struct comm_info* c_info, struct Bench* Bmark, MODES BMODE,
    int header, int size, struct iter_schedule* ITERATIONS,
    double *time);
/* >> IMB 3.1  */

/* New in IMB_3.0 */
void IMB_help();

void IMB_display_times(struct Bench* Bmark, double* tlist, struct comm_info* c_info,
                       int group, int n_sample, int size,
                       int edit_type);

/* IMB 3.1 << */
void IMB_show_selections(struct comm_info* c_info, struct Bench* BList, int *argc, char ***argv);
/* >> IMB 3.1  */


void IMB_calculate_times(int ntimes, struct comm_info* c_info, int group_id,
                         double *tlist, Timing* timing
#ifdef CHECK
                         , double *defect
#endif
    );

void IMB_show_procids(struct comm_info* c_info);

void IMB_print_array(int* Array, int N, int disp_N,
                     int M, char* txt, FILE* unit);

void IMB_print_int_row(FILE* unit, int* Array, int M);

void IMB_print_info();

void IMB_print_header(int out_format, struct Bench* bmark,
                      struct comm_info* c_info, MODES bench_mode);

void IMB_edit_format(int n_ints, int n_floats);

void IMB_make_line(int li_len);

void* IMB_v_alloc(size_t Len, char* where);

/*void IMB_i_alloc(int** B, size_t Len, char* where );  ==> define macro*/

void gpu_initialize();

int gpu_memcpy(void *dst, const void *src, size_t sz);

void IMB_alloc_buf(struct comm_info* c_info, char* where, size_t s_len,
                   size_t r_len);

void IMB_alloc_aux(size_t L, char* where);

void IMB_free_aux();

void IMB_v_free(void **B);

void IMB_ass_buf(void* buf, int rank, size_t pos1, size_t pos2, int value);

void IMB_set_buf(struct comm_info* c_info, int selected_rank, size_t s_pos1,
                 size_t s_pos2, size_t r_pos1, size_t r_pos2);

void IMB_init_pointers(struct comm_info *c_info);

/* IMB 3.1 << */
void IMB_init_buffers_iter(struct comm_info* c_info, struct iter_schedule* ITERATIONS,
struct Bench* Bmark, MODES BMODE, int iter, int size);

void IMB_free_all(struct comm_info* c_info, struct Bench** P_BList, struct iter_schedule* ITERATIONS);
/* >> IMB 3.1  */

void IMB_del_s_buf(struct comm_info* c_info);

void IMB_del_r_buf(struct comm_info* c_info);

char* IMB_str(const char* Bname);

void IMB_lwr(char* Bname);

int IMB_str_atoi(char s[]);

void IMB_str_erase(char* string, int Nblnc);

void IMB_err_hand(int ERR_IS_MPI, int ERR_CODE);

void IMB_errors_mpi(MPI_Comm * comm, int* ierr, ...);

#ifdef EXT
void IMB_errors_win(MPI_Win * WIN, int* ierr, ...);
#endif

#ifdef MPIIO
void IMB_errors_io(MPI_File * fh, int* ierr, ...);
#endif

void IMB_init_errhand(struct comm_info* c_info);

void IMB_set_errhand(struct comm_info* c_info);

void IMB_del_errhand(struct comm_info* c_info);

void IMB_chk_dadd(void* AUX, int Locsize, size_t buf_pos,
                  int rank0, int rank1);

double IMB_ddiff(assign_type *A, assign_type *B, size_t len,
                 size_t *fault_pos);

void IMB_show(char* text, struct comm_info* c_info, void* buf,
              size_t loclen, size_t totlen, int j_sample,
              POSITIONING fpos);

void IMB_err_msg(struct comm_info* c_info, char* text, size_t totsize,
                 int j_sample);

void IMB_chk_diff(char* text, struct comm_info* c_info, void* RECEIVED,
                  size_t buf_pos, int Locsize, size_t Totalsize,
                  int unit_size, DIRECTION mode, POSITIONING fpos,
                  int n_sample, int j_sample, int source,
                  double* diff);

void IMB_cmp_cat(struct comm_info *c_info, void* RECEIVED, size_t size,
                 size_t bufpos, int unit_size, int perm,
                 size_t* lengths, int*ranks, int* Npos,
                 size_t *faultpos, double* diff);

void IMB_chk_contiguous(struct comm_info *c_info, int* rdispl, int* sizes,
                        double*diff);

void IMB_chk_distr(struct comm_info *c_info, size_t size, int n_sample,
                   size_t* lengths, int* ranks, int Npos,
                   double *diff);

void IMB_chk_contained(void* part, size_t p_size, void* whole,
                       size_t w_size, size_t* pos, size_t* fpos,
                       double* D, char*msg);

long IMB_compute_crc(register char* buf, register size_t size);

#endif // IMB_PROTOTYPES_H
