/*****************************************************************************
 *                                                                           *
 * Copyright 2016-2019 Intel Corporation.                                    *
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
*/

#pragma once
#include <map>
#include "IMB_benchmark.h"
#include "IMB_comm_info.h"


extern "C" {
#include "IMB_prototypes.h"
}

#define GLUE_TYPENAME(A,B) A,B

#include "helper_IMB_functions.h"

#define GET_GLOBAL(TYPE, NAME) { TYPE *p = suite->get_parameter(#NAME).as<TYPE>(); \
                                 assert(p != NULL); \
                                 memcpy(&NAME, p, sizeof(TYPE)); }

#ifdef WIN_IMB

#include <windows.h>
#define DEFAULT_SLEEP_TIME_MILLISEC 100
#define MILLISEC_IN_SEC 1000

#define SLEEP(t)                                                   \
  do                                                               \
  {                                                                \
      if ((t * MILLISEC_IN_SEC) / 10 > DEFAULT_SLEEP_TIME_MILLISEC)\
          Sleep((t * MILLISEC_IN_SEC) / 10);                       \
      else                                                         \
          Sleep(DEFAULT_SLEEP_TIME_MILLISEC);                      \
  } while (0)

#else

#include <time.h>
#define DEFAULT_SLEEP_TIME_NANOSEC 100000000
#define NANOSEC_IN_SEC 1000000000

#define SLEEP(t)                                                            \
  do                                                                        \
  {                                                                         \
      struct timespec sleep_time;                                           \
      double t_sleep = t / 10;                                              \
      sleep_time.tv_sec = (int) t_sleep;                                    \
      sleep_time.tv_nsec = ((t_sleep - (int)t_sleep) * NANOSEC_IN_SEC);     \
      if (sleep_time.tv_sec == 0 &&                                         \
          sleep_time.tv_nsec < DEFAULT_SLEEP_TIME_NANOSEC)                  \
          sleep_time.tv_nsec = DEFAULT_SLEEP_TIME_NANOSEC;                  \
      nanosleep(&sleep_time, NULL);                                         \
  } while (0)

#endif

template <class bs, original_benchmark_func_t fn_ptr>
class OriginalBenchmark : public Benchmark {
    protected:
        static smart_ptr<Bmark_descr> descr;
        comm_info c_info;
        iter_schedule ITERATIONS;
        MODES BMODE;
        double time[MAX_TIME_ID];
        Bench BMark[1];

        int FULL_NP;
        int RANK;

        GLOBALS glob;
    public:
        using Benchmark::scope;
        virtual void allocate_internals() {
            BMark[0].name = NULL;
            if (descr.get() == NULL)
                descr = new Bmark_descr;
        }
        virtual bool init_description();
        virtual void init() {
            MPI_Comm_size(MPI_COMM_WORLD, &FULL_NP);
            MPI_Comm_rank(MPI_COMM_WORLD, &RANK);

            // Copy some global data from BenchmarkSuite
            GET_GLOBAL(comm_info, c_info);
            GET_GLOBAL(iter_schedule, ITERATIONS);
            GET_GLOBAL(GLOBALS, glob);

            assert(RANK == c_info.w_rank);
            assert(FULL_NP == c_info.w_num_procs);

            BMark->name = strdup(name);
            descr->IMB_set_bmark(BMark, fn_ptr);
            descr->helper_sync_globals_1(c_info, glob, BMark);
            descr->helper_sync_globals_2(c_info, glob, BMark);

            scope = descr->helper_init_scope(c_info, BMark, glob);

            // FIXME glob.NP is used inside helper_init_scope, it's easy to mess it up
            glob.NP = 0;
            initialized = true;
        }
        virtual void run(const scope_item &item) {
            int size = item.len;
            int np = item.np;
            int imod = *(item.extra_fields.as<int>());
            double t;
            MPI_Datatype base_s_dt, base_r_dt, base_red_dt;
            if (!initialized)
                return;
            if (descr->stop_iterations)
                return;
            if ((c_info.contig_type == CT_BASE_VEC || c_info.contig_type == CT_RESIZE_VEC) &&
                size != 0) {

                int size_dt;

                base_s_dt   = c_info.s_data_type;
                MPI_Type_size(base_s_dt, &size_dt);
                MPI_Type_vector(size / size_dt, 1, 1, base_s_dt, &(c_info.s_data_type));
                MPI_Type_commit(&(c_info.s_data_type));

                base_r_dt   = c_info.r_data_type;
                MPI_Type_size(base_r_dt, &size_dt);
                MPI_Type_vector(size / size_dt, 1, 1, base_r_dt, &(c_info.r_data_type));
                MPI_Type_commit(&(c_info.r_data_type));

                base_red_dt = c_info.red_data_type;
                MPI_Type_size(base_red_dt, &size_dt);
                MPI_Type_vector(size / size_dt, 1, 1, base_red_dt, &(c_info.red_data_type));
                MPI_Type_commit(&(c_info.red_data_type));
            }

            if (np != glob.NP || imod != glob.imod) {
                glob.NP = np;
                glob.imod = imod;
                if (!IMB_valid(&c_info, BMark, glob.NP)) {
                    descr->stop_iterations = true;
                    return;
                }
                IMB_init_communicator(&c_info, glob.NP);
#ifdef MPIIO
                if (IMB_init_file(&c_info, BMark, &ITERATIONS, glob.NP) != 0) IMB_err_hand(0, -1);
#endif /*MPIIO*/
                descr->helper_sync_globals_2(c_info, glob, BMark);
            }
            if( BMark->RUN_MODES[0].type == Sync ) {
                glob.iter = c_info.n_lens - 1;
            }
#ifdef MPIIO
            if(c_info.w_rank == 0 &&
               do_nonblocking_) {
                double MFlops = IMB_cpu_exploit_reworked(TARGET_CPU_SECS, 1);
                printf("\n\n# For nonblocking benchmarks:\n\n");
                printf("# Function CPU_Exploit obtains an undisturbed\n");
                printf("# performance of %7.2f MFlops\n",MFlops);
                do_nonblocking_ = 0;
            }
#endif
            glob.size = size;
            BMODE = &(BMark->RUN_MODES[imod]);
            descr->IMB_init_buffers_iter(&c_info, &ITERATIONS, BMark, BMODE, glob.iter, size);
            descr->helper_time_check(c_info, glob, BMark, ITERATIONS);
            bool failed = (descr->stop_iterations || (BMark->sample_failure));
            if (!failed) {
                IMB_warm_up(BMark, &c_info, size, &ITERATIONS, glob.iter);
                t = MPI_Wtime();
                fn_ptr(&c_info, size, &ITERATIONS, BMODE, time);
                t = MPI_Wtime() - t;
                MPI_Barrier(MPI_COMM_WORLD);
                SLEEP(t);
            }
            IMB_output(&c_info, BMark, BMODE, glob.header, size, &ITERATIONS, time);
            IMB_close_transfer(&c_info, BMark, size);
            if ((c_info.contig_type == CT_BASE_VEC || c_info.contig_type == CT_RESIZE_VEC) &&
                size != 0) {

                MPI_Type_free(&(c_info.s_data_type));
                c_info.s_data_type = base_s_dt;

                MPI_Type_free(&(c_info.r_data_type));
                c_info.r_data_type = base_r_dt;

                MPI_Type_free(&(c_info.red_data_type));
                c_info.red_data_type = base_red_dt;
            }
            IMB_del_s_buf(&c_info);
            IMB_del_r_buf(&c_info);
            glob.header = 0;
            glob.iter++;
        }
        virtual bool is_default() {
            return descr->is_default();
        }
        virtual std::vector<std::string> get_comments() {
            return descr->comments;
        }
        ~OriginalBenchmark() {
            free(BMark[0].name);
        }
        DEFINE_INHERITED(GLUE_TYPENAME(OriginalBenchmark<bs, fn_ptr>), bs);
};

