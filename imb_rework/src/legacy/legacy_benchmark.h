#pragma once
#include <map>
//#include "becnhmark_suites_collection.h"
//#include "utils.h"
//#include "benchmark_suite.h"
//#include "legacy_MPI1_suite.h"
#include "IMB_benchmark.h"
#include "IMB_comm_info.h"


extern "C" {
#include "IMB_prototypes.h"
}

#define GLUE_TYPENAME(A,B) A,B

#include "reworked_IMB_functions.h"

#define GET_GLOBAL(TYPE, NAME) { TYPE *p = (TYPE *)bs::get_internal_data_ptr(#NAME); memcpy(&NAME, p, sizeof(TYPE)); }

extern "C" { void IMB_Barrier(MPI_Comm comm); }

template <class bs, original_benchmark_func_t fn_ptr>
class OriginalBenchmark : public Benchmark {
    protected:
        static reworked_Bmark_descr *descr;
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
            if (descr == NULL) 
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

            // This is to do when change NP
            //if (!IMB_valid(&c_info, BMark, glob.NP))
            //    return;
            //IMB_init_communicator(&c_info, glob.NP);

            // FIXME glob.NP is used inside helper_init_scope, it's easy to mess it up
            glob.NP = 0;
            initialized = true;
        }
        virtual void run(const scope_item &item) {
            int size = item.len;
            int np = item.np; 
            if (!initialized)
                return;
            if (descr->stop_iterations)
                return;
            if (np != glob.NP) {
                glob.NP = np;
                if (!IMB_valid(&c_info, BMark, glob.NP)) {
                    descr->stop_iterations = true;
                    return;
                }
                IMB_init_communicator(&c_info, glob.NP);
                descr->helper_sync_legacy_globals_2(c_info, glob, BMark);
            }
            glob.size = size;
            BMODE = BMark->RUN_MODES;
            descr->IMB_init_buffers_iter(&c_info, &ITERATIONS, BMark, BMODE, glob.iter, size);
            descr->helper_time_check(c_info, glob, BMark, ITERATIONS);
            bool failed = (descr->stop_iterations || (BMark->sample_failure));
            if (!failed) {
                IMB_warm_up(&c_info, BMark, &ITERATIONS, glob.iter);
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
        ~OriginalBenchmark() {
            free(BMark[0].name);
            delete descr;
            descr = NULL;
        } 
        DEFINE_INHERITED(GLUE_TYPENAME(OriginalBenchmark<bs, fn_ptr>), bs);
};

