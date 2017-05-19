#include <mpi.h>
#include <omp.h>
#include <set>
#include <vector>
#include <string>
#include <map>
#include "benchmark.h"
#include "benchmark_suites_collection.h"
#include "utils.h"
#include "args_parser.h"

#include "halo_suite.h"

#include "layout.h"

namespace ndim_halo_benchmark {

static MPI_Comm immb_convert_comm(const char *, int mode_multiple, int thread_num) {
    MPI_Comm comm = MPI_COMM_WORLD, new_comm;
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


#include "MT_types.h"

// FIXME code duplication
/*
// convenience macros and functions working with input parameters 
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
    _ARRAY_DECL(count, int);
} immb_local_t;

enum malopt_t {
    MALOPT_SERIAL,
    MALOPT_CONTINOUS,
    MALOPT_PARALLEL
};

enum barropt_t {
    BARROPT_NOBARRIER,
    BARROPT_NORMAL,
    BARROPT_SPECIAL
};
*/

using namespace std;

namespace NS_HALO {
    immb_local_t *input;
    int mode_multiple;
    int stride;
    int num_threads;
    int rank, nranks;
    bool prepared = false;
    vector<int> cnt;
    int malloc_align;
    malopt_t malloc_option;
    barropt_t barrier_option;
    bool do_checks;
    MPI_Datatype datatype;
    int ndims;
    unsigned *mysubs;
    Layout L_global;
//------------
}

DECLARE_BENCHMARK_SUITE_STUFF(BS_GENERIC, ndim_halo_benchmark)

template <> void BenchmarkSuite<BS_GENERIC>::declare_args(args_parser &parser) const {
    parser.add_option_with_defaults<int>("stride", 0);
    parser.add_option_with_defaults<int>("warmup",  100);
    parser.add_option_with_defaults<int>("repeat", 1000);
    parser.add_option_with_defaults<string>("barrier", "on").
        set_caption("on|off|special");
    parser.add_option_with_defaults_vec<string>("comm", "world");
    parser.add_option_with_defaults_vec<int>("count", "1,2,4,8").
        set_mode(args_parser::option::APPLY_DEFAULTS_ONLY_WHEN_MISSING);
    parser.add_option_with_defaults<int>("malloc_align", 64);
    parser.add_option_with_defaults<string>("malloc_algo", "serial").
        set_caption("serial|continous|parallel");
    parser.add_option_with_defaults<bool>("check", false);
    parser.add_option_with_defaults<string>("datatype", "int").
        set_caption("int|char");

    parser.add_option_with_defaults_vec<int>("topo", "1", '.');
    parser.add_option_with_defaults_vec<int>("size", "1,2,4,8").
        set_mode(args_parser::option::APPLY_DEFAULTS_ONLY_WHEN_MISSING);
}

template <> bool BenchmarkSuite<BS_GENERIC>::prepare(const args_parser &parser, const set<string> &) {
    using namespace NS_HALO;

    parser.get_result_vec<int>("count", cnt);
    mode_multiple = (parser.get_result<string>("thread_level") == "multiple");
    stride = parser.get_result<int>("stride");

    string barrier_type = parser.get_result<string>("barrier");
    if (barrier_type == "off") barrier_option = BARROPT_NOBARRIER;
    else if (barrier_type == "on") barrier_option = BARROPT_NORMAL;
    else if (barrier_type == "special") barrier_option = BARROPT_SPECIAL;
    else {
        // FIXME get rid of cout some way!
        cout << "Wrong barrier option value" << endl;
        return false;
    }

    malloc_align = parser.get_result<int>("malloc_align");

    string malloc_algo = parser.get_result<string>("malloc_algo");
    if (malloc_algo == "serial") malloc_option = MALOPT_SERIAL;
    else if (malloc_algo == "continous") malloc_option = MALOPT_CONTINOUS;
    else if (malloc_algo == "parallel") malloc_option = MALOPT_PARALLEL;
    else {
        // FIXME get rid of cout some way!
        cout << "Wrong malloc_algo option value" << endl;
        return false;
    }
    if ((malloc_option == MALOPT_PARALLEL || malloc_option == MALOPT_CONTINOUS) && !mode_multiple) {
        malloc_option = MALOPT_SERIAL;
    }

    do_checks = parser.get_result<bool>("check");

    string dt = parser.get_result<string>("datatype");
    if (dt == "int") datatype = MPI_INT;
    else if (dt == "char") datatype = MPI_CHAR;
    else {
        // FIXME get rid of cout some way!
        cout << "Unknown data type in datatype option" << endl;
        return false;
    }

    if (do_checks && datatype != MPI_INT) {
        // FIXME get rid of cout some way!
        cout << "Only int data type is supported with check option" << endl;
        return false;
    }

    num_threads = 1;
    if (mode_multiple) {
#pragma omp parallel default(shared)
#pragma omp master
        num_threads = omp_get_num_threads();
    }
    input = (immb_local_t *)malloc(sizeof(immb_local_t) * num_threads);
    for (int thread_num = 0; thread_num < num_threads; thread_num++) {
        {
            size_t n = 1;
            _ARRAY_ALLOC(input[thread_num].comm, MPI_Comm, n);
            for (size_t i = 0; i < n; i++) {
                input[thread_num].comm[i] = immb_convert_comm("", mode_multiple, thread_num);
            }
        }
        {
            size_t n = cnt.size();
            _ARRAY_ALLOC(input[thread_num].count, int, n);
            for (size_t i = 0; i < n; i++) {
                input[thread_num].count[i] = cnt[i];
            }
        }
        input[thread_num].warmup = parser.get_result<int>("warmup");
        input[thread_num].repeat = parser.get_result<int>("repeat");
    }
    prepared = true;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    Layout &L = L_global;
    if (L.nranks() > nranks) {
        // FIXME get rid of cout some way!
        cout << "Not enough ranks, " << L.nranks() << " min. required" << endl;
        return false;
    }

    if (rank == 0)
        L.prlayout();
    MPI_Barrier(MPI_COMM_WORLD);

    ndims = L.ndims();
    //unsigned int mysubs[ndims];
    mysubs = (unsigned int *)malloc(ndims * sizeof(unsigned int));
    L.ranktosubs(rank, mysubs);
    L_global = L;
/*    
    //printf("%d: %d: ", rank, L.substorank(mysubs)); L.prsubs("mysubs", mysubs);
    enum { UP=0, DN }; const int ndirs = 2;
    enum { SEND=0, RECV }; const int nsr = 2;
    void *buffs[ndims][ndirs][nsr];
    for (int i = 0; i < ndims; ++i)
    for (int j = SEND; j <= RECV; ++j)
    for (int k = UP; k <= DN; ++k) {
        // aligned malloc of multiple of 2MiB is likely to get us a 2M huge page
        const int twomb = 2*1024*1024;
        size_t s = (L.size(i) + twomb - 1) & ~(twomb-1);
        buffs[i][j][k] = _mm_malloc(s, twomb);
        assert(buffs[i][j][k]);
        memset(buffs[i][j][k], rank, s);
    }
    int partner[ndims][ndirs];
    // construct the partners
    for (int dim = 0; dim < ndims; ++dim) {
        unsigned int partnersubs[ndims];
        for (int i = 0; i < ndims; ++i) partnersubs[i] = mysubs[i];
        partnersubs[dim] = (mysubs[dim]+1)%L.ranksperdim(dim);
        partner[dim][UP] = L.substorank(partnersubs);
        partnersubs[dim] = (L.ranksperdim(dim)+mysubs[dim]-1)%L.ranksperdim(dim);
        partner[dim][DN] = L.substorank(partnersubs);
    }

    #if 1
    for (int kk = 0; kk < nranks; ++kk) {
        if (kk == rank)
        for (int i = 0; i < ndims; ++i)
        for (int j = 0; j < ndirs; ++j) {
            printf("dim %d dir %c %d <=> %d\n", i, "UD"[j], rank, partner[i][j]);
        }
    }
    #endif
*/
    return true;
}

template <> void BenchmarkSuite<BS_GENERIC>::finalize(const set<string> &) {
    using namespace NS_HALO;
    for (int thread_num = 0; thread_num < num_threads; thread_num++) {
        _ARRAY_FREE(input[thread_num].comm);
    }
}

void *HALOBenchmarkSuite::get_internal_data_ptr(const std::string &key, int i) {
    using namespace NS_HALO;
    assert(prepared);
    if (key == "input[thread_num]") return &input[i];
    if (key == "num_threads") return &num_threads;
    if (key == "mode_multiple") return &mode_multiple;
    if (key == "stride") return &stride;
    if (key == "malloc_align") return &malloc_align;
    if (key == "malloc_option") return &malloc_option;
    if (key == "barrier_option") return &barrier_option;
    if (key == "do_checks") return &do_checks;
    if (key == "datatype") return &datatype;

    if (key == "ndims") return &ndims;
    if (key == "mysubs[i]") return &mysubs[i];
    if (key == "L") return &L_global;
    if (key == "rank") return &rank;
    if (key == "nranks") return &nranks;
    assert(false);
    return 0;
}

}

