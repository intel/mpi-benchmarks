#pragma once
#include <mpi.h>
#include <iostream>

#include "benchmark.h"
#include "benchmark_suite.h"

#include "immb.h"

using namespace std;

#define GLUE_TYPENAME(A,B) A,B
#define GET_GLOBAL_VEC(TYPE, NAME, i) { TYPE *p = (TYPE *)bs::get_internal_data_ptr(#NAME, i); memcpy(&NAME, p, sizeof(TYPE)); }
#define GET_GLOBAL(TYPE, NAME) { TYPE *p = (TYPE *)bs::get_internal_data_ptr(#NAME); memcpy(&NAME, p, sizeof(TYPE)); }

void simple_barrier()
{
    MPI_Barrier(MPI_COMM_WORLD);
}

void omp_aware_barrier()
{
#pragma omp barrier    
    MPI_Barrier(MPI_COMM_WORLD);
#pragma omp barrier    
}

void no_barrier()
{
}

typedef void (*barrier_func_t)();

struct input_benchmark_data {
    struct {
        int root;
    } collective;
    struct {
        int stride;
    } pt2pt;
    struct {
        int *cnt;
        int *displs;
    } collective_vector;
    struct {
        barrier_func_t fn_ptr;
    } barrier;
};

struct output_benchmark_data {
    struct {
        double *time_ptr;
    } timing;
};

typedef int (*mt_benchmark_func_t)(int repeat, void *in, void *out, int count,
                                   MPI_Datatype type, MPI_Comm comm, int ranks, int size, 
                                   input_benchmark_data *data, output_benchmark_data *odata);

template <class bs, mt_benchmark_func_t fn_ptr>
class BenchmarkMT : public Benchmark {
    public:    
    enum Flags {
        COLLECTIVE,
        PT2PT,
        SEPARATE_MEASURING,
        COLLECTIVE_VECTOR,
        SEND_TO_ALL,
        RECV_FROM_ALL,
        SEND_TO_TWO,
        RECV_FROM_TWO
    };
    std::set<Flags> flags; 
    std::vector<char *> a;
    std::vector<char *> b;
    std::vector<input_benchmark_data *> idata;
    std::vector<output_benchmark_data *> odata;
    immb_local_t *input;
    int mode_multiple;
    int stride;
    int num_threads;
    double time_avg, time_min, time_max;
    int world_rank, world_size;
    public:
    virtual void init_flags();
    virtual void run_instance(immb_local_t *input, int count, double &t, int &result) {
        MPI_Comm comm = _ARRAY_THIS(input->comm);
        int warmup = input->warmup, repeat = input->repeat;
        if (repeat <= 0) return;
        int rank, size;
        MPI_Comm_rank(comm, &rank);
        MPI_Comm_size(comm, &size);
        char *in = a[omp_get_thread_num()];
        char *out = b[omp_get_thread_num()];
        input_benchmark_data &idata_local = *idata[omp_get_thread_num()];
        output_benchmark_data &odata_local = *odata[omp_get_thread_num()];
        idata_local.collective.root = 0;
        idata_local.pt2pt.stride = stride;
        barrier_func_t bfn;
        if(input->barrier) {
            if(input->global->mode_multiple) {
                bfn = omp_aware_barrier;
            } else {
                bfn = simple_barrier;
            }
        } else {
            bfn = no_barrier;
        }
        if (flags.count(SEPARATE_MEASURING)) {
            idata_local.barrier.fn_ptr = bfn;
            if (flags.count(COLLECTIVE_VECTOR)) {
                for (int i = 0; i < size; i++) {
                    idata_local.collective_vector.cnt[i] = count;
                    idata_local.collective_vector.displs[i] = count * i;
                }
            }
            odata_local.timing.time_ptr = NULL;
            fn_ptr(warmup, in, out, count, MPI_CHAR, comm, rank, size, &idata_local, &odata_local);
            odata_local.timing.time_ptr = &t;
            result = fn_ptr(repeat, in, out, count, MPI_CHAR, comm, rank, size, &idata_local, &odata_local);
        } else {
            odata_local.timing.time_ptr = NULL;
            fn_ptr(warmup, in, out, count, MPI_CHAR, comm, rank, size, &idata_local, &odata_local);
            bfn();
            t = MPI_Wtime();
            result = fn_ptr(repeat, in, out, count, MPI_CHAR, comm, rank, size, &idata_local, &odata_local);
            t = MPI_Wtime()-t;
        }
        if (!result)
            t = 0;
        return;
    }
    virtual void init() {
        init_flags();
        GET_GLOBAL(int, mode_multiple);
        GET_GLOBAL(int, stride);
        GET_GLOBAL(int, num_threads);
        input = (immb_local_t *)malloc(sizeof(immb_local_t) * num_threads);
        for (int thread_num = 0; thread_num < num_threads; thread_num++) {
            GET_GLOBAL_VEC(immb_local_t, input[thread_num], thread_num);
        }
        VarLenScope *sc = new VarLenScope(input[0].count, input[0].countn);
        scope = sc;

        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

        // get longest element from sequence
        size_t maxlen = sc->get_max_len();
        size_t size_a = sizeof(char)*maxlen;
        size_t size_b = sizeof(char)*maxlen;
        if (flags.count(SEND_TO_ALL))
            size_a *= world_size;
        else if (flags.count(SEND_TO_TWO))
            size_a *= 2;
        if (flags.count(RECV_FROM_ALL))
            size_b *= world_size;
        else if (flags.count(RECV_FROM_TWO))
            size_b *= 2;

        for (int thread_num = 0; thread_num < num_threads; thread_num++) {
            a.push_back((char *)malloc(size_a));
            b.push_back((char *)malloc(size_b));
            idata.push_back((input_benchmark_data *)malloc(sizeof(input_benchmark_data)));
            odata.push_back((output_benchmark_data *)malloc(sizeof(output_benchmark_data)));
            if (flags.count(COLLECTIVE_VECTOR)) {
                idata[idata.size()-1]->collective_vector.cnt = (int *)malloc(world_size * sizeof(int));
                idata[idata.size()-1]->collective_vector.displs = (int *)malloc(world_size * sizeof(int));
            }        
        }
   }
    virtual void run(const std::pair<int, int> &p) { 
        double t, tavg = 0, tmin = 0, tmax = 0; 
        int nresults = 0;
        if (mode_multiple) {
        #pragma omp parallel default(shared)
            {
                double t_mp;
                int result;
                run_instance(&input[omp_get_thread_num()], p.second, t_mp, result);
            #pragma omp critical
                {
                    tmax = max(tmax, t_mp);
                    tmin = min(tmin, t_mp);
                    tavg = tavg + t_mp;
                    nresults += result;
                }
            }
        } else {
            run_instance(&input[0], p.second, t, nresults);
            tavg = tmax = tmin = t;
        }
        MPI_Allreduce(&tavg, &time_avg, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(&tmin, &time_min, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
        MPI_Allreduce(&tmax, &time_max, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
        MPI_Allreduce(MPI_IN_PLACE, &nresults, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        if (nresults) {
            time_avg /= (double)nresults;
        }
        time_avg /= (double)input[0].repeat;
        time_min /= (double)input[0].repeat;
        time_max /= (double)input[0].repeat;
        if (world_rank == 0) {
            if (nresults)
                printf("pattern: %s, count: %d, time: %f usec\n",
                       get_name().c_str(), p.second, 1.e6 * time_avg);
            else
                printf("No successful executions\n");
        }
    }
    virtual void finalize() {
        free(input);
        for (int thread_num = 0; thread_num < num_threads; thread_num++) {
            free(a[thread_num]);
            free(b[thread_num]);
            if (flags.count(COLLECTIVE_VECTOR)) {
               free(idata[thread_num]->collective_vector.cnt);
               free(idata[thread_num]->collective_vector.displs);
            }
        }
    }
    DEFINE_INHERITED(GLUE_TYPENAME(BenchmarkMT<bs, fn_ptr>), bs);
};

