#pragma once
#include <mpi.h>
#include <omp.h>

#include <iostream>

#include "benchmark.h"
#include "benchmark_suite.h"


// FIXME!! code duplication
/* convenience macros and functions working with input parameters */
#define _ARRAY_DECL(_array,_type) _type *_array; int _array##n; int _array##i
#define _ARRAY_ALLOC(_array,_type,_size) _array##i=0; _array=(_type*)malloc((_array##n=_size)*sizeof(_type))
#define _ARRAY_FREE(_array) free(_array)
#define _ARRAY_FOR(_array,_i,_action) for(_i=0;_i<_array##n;_i++) {_action;}
#define _ARRAY_NEXT(_array) if(++_array##i >= _array##n) _array##i = 0; else return 1
#define _ARRAY_THIS(_array) (_array[_array##i])
#define _ARRAY_CHECK(_array) (_array##i >= _array##n)

typedef struct _immb_local_t {
        int warmup;
            int repeat;
                _ARRAY_DECL(comm, MPI_Comm);
} immb_local_t;



#define MALLOC_OPT 4

using namespace std;

#define GLUE_TYPENAME(A,B) A,B
#define GET_GLOBAL_VEC(TYPE, NAME, i) { TYPE *p = (TYPE *)bs::get_internal_data_ptr(#NAME, i); memcpy(&NAME, p, sizeof(TYPE)); }
#define GET_GLOBAL(TYPE, NAME) { TYPE *p = (TYPE *)bs::get_internal_data_ptr(#NAME); memcpy(&NAME, p, sizeof(TYPE)); }

#if MALLOC_OPT == 3 ||  MALLOC_OPT == 4
template <typename T>
class Allocator {
    protected:
    std::vector<T *> original_ptrs;
    public:
    virtual T *Alloc(size_t size) = 0;
    Allocator() {}
    virtual ~Allocator() { 
        for (size_t i = 0; i < original_ptrs.size(); i++) { 
            free(original_ptrs[i]); 
        } 
    }
    private:
    Allocator &operator=(const Allocator &) { return *this; }
    Allocator(const Allocator &) {}
};

template <typename T>
class AlignedAllocator : public Allocator<T> {
    public:
    size_t align;
    AlignedAllocator(size_t _align = 1) : align(_align) { }
    virtual T *Alloc(size_t size) {
        //size_t aligned_size = (size + align - 1) & ~(align - 1);
        //assert(aligned_size >= size);
        size_t size_with_spare_space = size + align;
        T *ptr = (T *)malloc(size_with_spare_space);
        Allocator<T>::original_ptrs.push_back(ptr);
        size_t diff = align - ((size_t)(ptr) % (size_t)align);
        return ptr + diff;
    }
    virtual ~AlignedAllocator() {};
};
#endif

static void simple_barrier()
{
    MPI_Barrier(MPI_COMM_WORLD);
}

static void special_barrier()
{
    int size = 0;
    int rank = 0;

    int mask = 0x1;
    int dst,src;

    int tmp = 0;

    MPI_Comm_size(MPI_COMM_WORLD, &size );
    MPI_Comm_rank(MPI_COMM_WORLD, &rank );

    for( ; mask < size; mask <<=1 ) {
        dst = (rank + mask) % size;
        src = (rank - mask + size) % size;
        MPI_Sendrecv( &tmp, 0, MPI_BYTE, dst, 1010,
                      &tmp, 0, MPI_BYTE, src, 1010,
                      MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}

static void omp_aware_barrier()
{
#pragma omp barrier    
    MPI_Barrier(MPI_COMM_WORLD);
#pragma omp barrier    
}

static void no_barrier()
{
}

typedef void (*barrier_func_t)();

struct input_benchmark_data {
    struct {
        int stride;
    } pt2pt;
};

struct output_benchmark_data {
    struct {
        double *time_ptr;
    } timing;
};

typedef int (*halo_benchmark_func_t)(int repeat, int skip, void *in, void *out, int count,
                                   MPI_Datatype type, MPI_Comm comm, int ranks, int size, 
                                   input_benchmark_data *data, output_benchmark_data *odata);

template <class bs, halo_benchmark_func_t fn_ptr>
class BenchmarkHALO : public Benchmark {
    public:    
    enum Flags {
        DUMMY
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
        idata_local.pt2pt.stride = stride;
        odata_local.timing.time_ptr = NULL;
        fn_ptr(warmup, 0, in, out, count, MPI_CHAR, comm, rank, size, &idata_local, &odata_local);
        t = MPI_Wtime();
        result = fn_ptr(repeat, 0, in, out, count, MPI_CHAR, comm, rank, size, &idata_local, &odata_local);
        t = MPI_Wtime()-t;
        if (!result)
            t = 0;
        return;
    }
    virtual void init() {
        init_flags();
        GET_GLOBAL(int, mode_multiple);
        GET_GLOBAL(int, num_threads);
        input = (immb_local_t *)malloc(sizeof(immb_local_t) * num_threads);
        for (int thread_num = 0; thread_num < num_threads; thread_num++) {
            GET_GLOBAL_VEC(immb_local_t, input[thread_num], thread_num);
        }
        int c[1] = {1024};
        VarLenScope *sc = new VarLenScope(c, 1);
        scope = sc;

        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

        // get longest element from sequence
        size_t maxlen = sc->get_max_len();
        size_t size_a = sizeof(char) * maxlen;
        size_t size_b = sizeof(char) * maxlen;

#if MALLOC_OPT == 1        
        for (int thread_num = 0; thread_num < num_threads; thread_num++) {
            a.push_back((char *)malloc(size_a));
            b.push_back((char *)malloc(size_b));
        }
#elif MALLOC_OPT == 2
        a.resize(num_threads);
        b.resize(num_threads);
#pragma omp parallel
        {
            a[omp_thread_num()] = (char *)malloc(size_a);
            b[omp_thread_num()] = (char *)malloc(size_b);
        }
#elif MALLOC_OPT == 3
        static AlignedAllocator<char> allocator(64);
        for (int thread_num = 0; thread_num < num_threads; thread_num++) {
            a.push_back((char *)allocator.Alloc(size_a));
            b.push_back((char *)allocator.Alloc(size_b));
        }
#elif MALLOC_OPT == 4
        static AlignedAllocator<char> allocator(64);
        char *a_base = (char *)allocator.Alloc(size_a * num_threads);
        char *b_base = (char *)allocator.Alloc(size_b * num_threads);
        for (int thread_num = 0; thread_num < num_threads; thread_num++) {
            a.push_back(a_base + (size_t)thread_num * size_a);
            b.push_back(b_base + (size_t)thread_num * size_b);
        }
#endif        
        for (int thread_num = 0; thread_num < num_threads; thread_num++) {
            idata.push_back((input_benchmark_data *)malloc(sizeof(input_benchmark_data)));
            odata.push_back((output_benchmark_data *)malloc(sizeof(output_benchmark_data)));
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
#if MALLOC_OPT != 3
        for (int thread_num = 0; thread_num < num_threads; thread_num++) {
            free(a[thread_num]);
            free(b[thread_num]);
        }
#endif
    }
    DEFINE_INHERITED(GLUE_TYPENAME(BenchmarkHALO<bs, fn_ptr>), bs);
};

