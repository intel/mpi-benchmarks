#include <mpi.h>
#include <omp.h>
#include <set>
#include <vector>
#include <string>
#include <map>
#include "benchmark.h"
#include "benchmark_suite.h"
#include "utils.h"

#include "halo_suite.h"

static MPI_Comm immb_convert_comm(const char *str, int mode_multiple, int thread_num) {
    MPI_Comm comm = strcmp(str, "world") ? MPI_COMM_NULL : MPI_COMM_WORLD, new_comm;
    if(mode_multiple) {
        MPI_Info info;
        char ep_idx[128];
        sprintf(ep_idx, "%d", thread_num);
        MPI_Info_create(&info);
        MPI_Info_set(info, "ep_idx", ep_idx);
        MPI_Comm_dup(comm, &new_comm);
        MPI_Comm_set_info(new_comm, info);
        MPI_Info_free(&info);
        return new_comm;
    }
    return comm;
}


// FIXME code duplication
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


using namespace std;

namespace NS_HALO {
    immb_local_t *input;
    int mode_multiple;
    int stride;
    int num_threads;
    vector<int> sizes;
//    vector<string> comm_opts;
};

template<> map<string, const Benchmark*, set_operations::case_insens_cmp> *BenchmarkSuite<BS_HALO>::pnames = 0;
template<> BenchmarkSuite<BS_HALO> *BenchmarkSuite<BS_HALO>::instance = 0;

template <> const std::string BenchmarkSuite<BS_HALO>::get_name() const { return "IMB-HALO"; }

template <> void BenchmarkSuite<BS_HALO>::declare_args(args_parser &parser) const {
    parser.add_option_with_defaults<int>("warmup",  100);
    parser.add_option_with_defaults<int>("repeat", 1000);
    parser.add_option_with_defaults_vec<int>("topo", "1", '.');
    parser.add_option_with_defaults_vec<int>("size", "1,2,4,8").
        set_mode(args_parser::option::APPLY_DEFAULTS_ONLY_WHEN_MISSING);
}

template <> bool BenchmarkSuite<BS_HALO>::prepare(const args_parser &parser, const set<string> &benchs) {
    using namespace NS_HALO;
    parser.get_result_vec<int>("size", sizes);
    mode_multiple = (parser.get_result<string>("thread_level") == "multiple");
    num_threads = 1;
    if (mode_multiple) {
#pragma omp parallel default(shared)
#pragma omp master        
        num_threads = omp_get_num_threads();
    } 
    input = (immb_local_t *)malloc(sizeof(immb_local_t) * num_threads);
    for (int thread_num = 0; thread_num < num_threads; thread_num++) {
        {
            size_t n = 1; //comm_opts.size();
            _ARRAY_ALLOC(input[thread_num].comm, MPI_Comm, n);
            for (size_t i = 0; i < n; i++) {
                input[thread_num].comm[i] = immb_convert_comm("world", mode_multiple, thread_num);
            }
        }
        input[thread_num].warmup = parser.get_result<int>("warmup");
        input[thread_num].repeat = parser.get_result<int>("repeat");
    }
    return true;
}

template <> void BenchmarkSuite<BS_HALO>::finalize(const set<string> &benchs) {
    using namespace NS_HALO;
    for (int thread_num = 0; thread_num < num_threads; thread_num++) {
        _ARRAY_FREE(input[thread_num].comm);
    }
}


void *HALOBenchmarkSuite::get_internal_data_ptr(const std::string &key, int i) {
    using namespace NS_HALO;
    if (key == "input[thread_num]") return &input[i];
    if (key == "num_threads") return &num_threads;
    if (key == "mode_multiple") return &mode_multiple;
    assert(false);
    return 0;
}
