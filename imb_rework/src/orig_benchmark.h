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

typedef void (*original_benchmark_func_t)(struct comm_info* c_info, int size, 
        struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time);

//void (*)(comm_info *, int, iter_schedule *, MODES, double *)

#include "reworked_IMB_functions.h"

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
        int NP;
        int rank;
    public:
        static bool init_descr();
        virtual void init() {
            if (!init_descr()) {
                throw logic_error("OriginalBenchmark: something is wrong with a benchmark description");
            }
            comm_info *p1 = (comm_info *)bs::get_internal_data_ptr("c_info");
            memcpy(&c_info, p1, sizeof(comm_info));
            iter_schedule *p2 = (iter_schedule *)bs::get_internal_data_ptr("ITERATIONS");
            memcpy(&ITERATIONS, p2, sizeof(ITERATIONS));
            BMark->name = strdup(name); //FIXME memleak here
            descr.IMB_set_bmark(BMark, fn_ptr);
            MPI_Comm_size(MPI_COMM_WORLD, &NP);
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            IMB_init_communicator(&c_info, NP);
            if (rank == 0) {
                cout << "I am " << name << " benchmark wrapper and I've got benchmark function ptr: " << long(fn_ptr) << endl;
            }
            // MPI_Type_size
        }
        virtual void run(int iter, int size) { 
            BMODE = BMark->RUN_MODES;
            descr.IMB_init_buffers_iter(&c_info, &ITERATIONS, BMark, BMODE, iter, size);
            IMB_warm_up(&c_info, BMark, &ITERATIONS, iter);
            fn_ptr(&c_info, size, &ITERATIONS, BMODE, time);
            if (rank == 0) {
                cout << "time: " << time[0] << endl;
            }
        } 
        DEFINE_INHERITED(GLUE_TYPENAME(OriginalBenchmark<bs, fn_ptr>), bs);
};

