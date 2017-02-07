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

//typedef void (*original_benchmark_func_t)(struct comm_info* c_info, int size, 
//        struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time);

//void (*)(comm_info *, int, iter_schedule *, MODES, double *)

#include "reworked_IMB_functions.h"
/*
struct LEGACY_GLOBALS {
    int NP, iter, size, ci_np;
    int header, MAXMSG;
    int x_sample, n_sample;
    Type_Size unit_size;
    double time[MAX_TIME_ID];
};
*/

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

        LEGACY_GLOBALS glob; //int NP, iter, size, unit_size, ci_np;
    public:
        static bool init_descr();
        virtual void init() {
            MPI_Comm_size(MPI_COMM_WORLD, &FULL_NP);
            MPI_Comm_rank(MPI_COMM_WORLD, &RANK);
            if (!init_descr()) {
                throw logic_error("OriginalBenchmark: something is wrong with a benchmark description");
            }
            comm_info *p1 = (comm_info *)bs::get_internal_data_ptr("c_info");
            memcpy(&c_info, p1, sizeof(comm_info));
            iter_schedule *p2 = (iter_schedule *)bs::get_internal_data_ptr("ITERATIONS");
            memcpy(&ITERATIONS, p2, sizeof(ITERATIONS));
            LEGACY_GLOBALS *p3 = (LEGACY_GLOBALS *)bs::get_internal_data_ptr("glob");
            memcpy(&glob, p3, sizeof(LEGACY_GLOBALS));

            assert(RANK == c_info.w_rank);
            assert(FULL_NP == c_info.w_num_procs);
 
            BMark->name = strdup(name); //FIXME memleak here
            descr.IMB_set_bmark(BMark, fn_ptr);
            descr.helper_sync_legacy_globals(c_info, glob, BMark);
            if (!IMB_valid(&c_info, BMark, glob.NP))
                return;
            IMB_init_communicator(&c_info, glob.NP);
//            if (RANK == 0) {
//                cout << "I am " << name << " benchmark wrapper and I've got benchmark function ptr: " << long(fn_ptr) << endl;
//            }
        }
        virtual void run() { 
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
            IMB_output(&c_info, BMark, BMODE, glob.header, glob.size, &ITERATIONS, time);
            IMB_close_transfer(&c_info, BMark, glob.size);
            descr.helper_post_step(glob, BMark);
        } 
        DEFINE_INHERITED(GLUE_TYPENAME(OriginalBenchmark<bs, fn_ptr>), bs);
};

