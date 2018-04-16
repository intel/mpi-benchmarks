/*****************************************************************************
 *                                                                           *
 * Copyright 2016-2018 Intel Corporation.                                    *
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

#include "reworked_IMB_functions.h"

#define GET_GLOBAL(TYPE, NAME) { TYPE *p = suite->get_parameter(#NAME).as<TYPE>(); \
                                 assert(p != NULL); \
                                 memcpy(&NAME, p, sizeof(TYPE)); }


extern "C" { void IMB_Barrier(MPI_Comm comm); }

template <class bs, original_benchmark_func_t fn_ptr>
class OriginalBenchmark : public Benchmark {
    protected:
        static smart_ptr<reworked_Bmark_descr> descr;
        comm_info c_info;
        iter_schedule ITERATIONS;
        MODES BMODE;
        double time[MAX_TIME_ID];
        Bench BMark[1];

        int FULL_NP;
        int RANK;

        LEGACY_GLOBALS glob;
    public:
        using Benchmark::scope;
        virtual void allocate_internals() { 
            BMark[0].name = NULL;
            if (descr.get() == NULL) 
                descr = new reworked_Bmark_descr;
        }
        virtual bool init_description();
        virtual void init() {
            MPI_Comm_size(MPI_COMM_WORLD, &FULL_NP);
            MPI_Comm_rank(MPI_COMM_WORLD, &RANK);

            // Copy some global data from BenchmarkSuite
            GET_GLOBAL(comm_info, c_info);
            GET_GLOBAL(iter_schedule, ITERATIONS);
            GET_GLOBAL(LEGACY_GLOBALS, glob);

            assert(RANK == c_info.w_rank);
            assert(FULL_NP == c_info.w_num_procs);
 
            BMark->name = strdup(name);
            descr->IMB_set_bmark(BMark, fn_ptr);
            descr->helper_sync_legacy_globals_1(c_info, glob, BMark);
            descr->helper_sync_legacy_globals_2(c_info, glob, BMark);

            scope = descr->helper_init_scope(c_info, BMark, glob);

            // FIXME glob.NP is used inside helper_init_scope, it's easy to mess it up
            glob.NP = 0;
            initialized = true;
        }
        virtual void run(const scope_item &item) {
            int size = item.len;
            int np = item.np; 
            int imod = *(item.extra_fields.as<int>());
            if (!initialized)
                return;
            if (descr->stop_iterations)
                return;
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
                descr->helper_sync_legacy_globals_2(c_info, glob, BMark);
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
                fn_ptr(&c_info, size, &ITERATIONS, BMODE, time);
            }
            MPI_Barrier(MPI_COMM_WORLD);
            IMB_output(&c_info, BMark, BMODE, glob.header, size, &ITERATIONS, time);
            IMB_close_transfer(&c_info, BMark, size);
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

