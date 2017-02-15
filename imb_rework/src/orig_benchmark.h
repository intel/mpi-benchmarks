#pragma once
#include "benchmark_suite.h"
#include "benchmark_suite_MPI1.h"
//extern "C" {
#include "IMB_benchmark.h"
#include "IMB_comm_info.h"
//}


extern "C" {
#include "IMB_prototypes.h"
}

//extern "C" {
//#include "original_imb/src/IMB_parse_name_mpi1.c"
//}

using namespace std;

#define GLUE_TYPENAME(A,B) A,B

#include "reworked_IMB_functions.h"

#define GET_GLOBAL(TYPE, NAME) { TYPE *p = (TYPE *)bs::get_internal_data_ptr(#NAME); memcpy(&NAME, p, sizeof(TYPE)); }

extern "C" { void IMB_Barrier(MPI_Comm comm); }

template <class bs, original_benchmark_func_t fn_ptr>
class OriginalBenchmark : public Benchmark {
    protected:
        static const char *name;
        static reworked_Bmark_descr descr;
        comm_info c_info;
        iter_schedule ITERATIONS;
        MODES BMODE;
        double time[MAX_TIME_ID];
        Bench BMark[1];

        int FULL_NP;
        int RANK;

        LEGACY_GLOBALS glob;
    public:
        virtual bool init_description();
        virtual void init() {
            MPI_Comm_size(MPI_COMM_WORLD, &FULL_NP);
            MPI_Comm_rank(MPI_COMM_WORLD, &RANK);

            // Copy some global data from BenchmarkSuite
            GET_GLOBAL(comm_info, c_info);
            GET_GLOBAL(iter_schedule, ITERATIONS);
            GET_GLOBAL(LEGACY_GLOBALS, glob);

//            comm_info *p1 = (comm_info *)bs::get_internal_data_ptr("c_info");
//            memcpy(&c_info, p1, sizeof(comm_info));
//            iter_schedule *p2 = (iter_schedule *)bs::get_internal_data_ptr("ITERATIONS");
//            memcpy(&ITERATIONS, p2, sizeof(ITERATIONS));
//            LEGACY_GLOBALS *p3 = (LEGACY_GLOBALS *)bs::get_internal_data_ptr("glob");
//            memcpy(&glob, p3, sizeof(LEGACY_GLOBALS));

            assert(RANK == c_info.w_rank);
            assert(FULL_NP == c_info.w_num_procs);
 
            BMark->name = strdup(name);
            descr.IMB_set_bmark(BMark, fn_ptr);
            descr.helper_sync_legacy_globals(c_info, glob, BMark);
            if (!IMB_valid(&c_info, BMark, glob.NP))
                return;
            IMB_init_communicator(&c_info, glob.NP);
            initialized = true;
        }
        virtual void run() { 
            if (!initialized)
                return;
            if (!descr.helper_is_next_iter(c_info, glob))
                return;
            descr.helper_get_next_size(c_info, glob);
            descr.helper_adjust_size(c_info, glob);
            BMODE = BMark->RUN_MODES;
            descr.IMB_init_buffers_iter(&c_info, &ITERATIONS, BMark, BMODE, glob.iter, glob.size);
            bool failed = descr.helper_time_check(c_info, glob, BMark, ITERATIONS);
            if (!failed) {
                IMB_warm_up(&c_info, BMark, &ITERATIONS, glob.iter);
                fn_ptr(&c_info, glob.size, &ITERATIONS, BMODE, time);
//                if (RANK == 0) {
//                    cout << "time: " << time[0] << endl;
//                }
            }
            MPI_Barrier(MPI_COMM_WORLD);
//            IMB_Barrier(MPI_COMM_WORLD);
            IMB_output(&c_info, BMark, BMODE, glob.header, glob.size, &ITERATIONS, time);
            IMB_close_transfer(&c_info, BMark, glob.size);
            descr.helper_post_step(glob, BMark);
        }
        virtual bool is_default() {
            return descr.is_default();
        }
        ~OriginalBenchmark() {
            free(BMark[0].name);
        } 
        OriginalBenchmark<bs, fn_ptr>() { bs::register_elem(this); BMark[0].name = NULL; }
        DEFINE_INHERITED(GLUE_TYPENAME(OriginalBenchmark<bs, fn_ptr>), bs);
};

