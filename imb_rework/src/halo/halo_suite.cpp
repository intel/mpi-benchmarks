#include <mpi.h>
#include <omp.h>
#include <set>
#include <vector>
#include <string>
#include <map>
#include <math.h>
#include <stdio.h>
#include "utils.h"
#include "benchmark.h"
#include "benchmark_suites_collection.h"
#include "args_parser.h"

namespace ndim_halo_benchmark {

#include "benchmark_suite.h"


template <typename integer>
integer gcd(integer a, integer b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    if (a == 0) return b;
    while (b != 0) {
        integer remainder = a % b;
        a = b;
        b = remainder;
    }
    return a;
}

#include "MT_types.h"

namespace NS_HALO {
    std::vector<thread_local_data_t> input;
    int mode_multiple;
    int stride;
    int num_threads;
    int rank, nranks;
    bool prepared = false;
    std::vector<int> count;
    int malloc_align;
//    malopt_t malloc_option;
//    barropt_t barrier_option;
    bool do_checks;
    MPI_Datatype datatype;
    int required_nranks, ndims;
    std::vector<int> ranksperdim;
    std::vector<int> mults;
    std::vector<unsigned int> mysubs;
/*    
    static void prvals(const std::vector<int> &arr)
    {
        for (int i = 0; i < ndims-1; ++i)
            printf("%d.", arr[i]);
        printf("%d", arr[ndims-1]);
    }    
    static void prlayout()
    {
        printf("%d dims, %d ranks, layout: ", ndims, required_nranks);
        prvals(ranksperdim);
        printf("\nmults: ");
        prvals(mults);
        printf("\n");
    }
    static void prsubs(const char *name, const std::vector<unsigned int> &subs)
    {
        printf("%s:", name);
        for (int i = 0; i < ndims; ++i)
            printf(" %d", subs[i]);
        printf("\n");
    }
*/    
    static void fill_in(std::vector<int> &topo)
    {
        ndims = topo.size();
        ranksperdim = topo;
        {
            int n = 0;
            for (int i = 0; i < ndims; ++i) {
                n = gcd(n, topo[i]);
            }
            assert(n > 0);
            for (int i = 0; i < ndims; ++i) {
                ranksperdim[i] = topo[i] / n;
            }
        }
        required_nranks = 1;
        for (int i = 0; i < ndims; ++i)
            required_nranks *= ranksperdim[i];
        if (nranks / required_nranks >= (1<<ndims)) {
            int mult = (int)(pow(nranks, 1.0/ndims));
            for (int i = 0; i < ndims; ++i)
                ranksperdim[i] *= mult;
            required_nranks = 1;
            for (int i = 0; i < ndims; ++i)
                required_nranks *= ranksperdim[i];
        }
        mults.resize(ndims);
        mults[ndims - 1] = 1;
        for (int i = ndims - 2; i >= 0; --i)
            mults[i] = mults[i + 1] * ranksperdim[i + 1];
    }
}

DECLARE_BENCHMARK_SUITE_STUFF(BS_GENERIC, ndim_halo_benchmark)

template <> void BenchmarkSuite<BS_GENERIC>::declare_args(args_parser &parser) const {
    parser.add_option_with_defaults<int>("stride", 0);
    parser.add_option_with_defaults<int>("warmup",  100);
    parser.add_option_with_defaults<int>("repeat", 1000);
    parser.add_option_with_defaults_vec<int>("count", "1,2,4,8").
        set_mode(args_parser::option::APPLY_DEFAULTS_ONLY_WHEN_MISSING);
    parser.add_option_with_defaults<int>("malloc_align", 64);
//    parser.add_option_with_defaults<bool>("check", false);
    parser.add_option_with_defaults<std::string>("datatype", "int").
        set_caption("int|char");

    parser.add_option_with_defaults_vec<int>("topo", "1", '.');
}

template <> bool BenchmarkSuite<BS_GENERIC>::prepare(const args_parser &parser, const std::set<std::string> &) {
    using namespace NS_HALO;

    parser.get_result_vec<int>("count", count);
    mode_multiple = (parser.get_result<std::string>("thread_level") == "multiple");
    stride = parser.get_result<int>("stride");

    malloc_align = parser.get_result<int>("malloc_align");

//    do_checks = parser.get_result<bool>("check");

    std::string dt = parser.get_result<std::string>("datatype");
    if (dt == "int") datatype = MPI_INT;
    else if (dt == "char") datatype = MPI_CHAR;
    else {
        // FIXME get rid of cout some way!
        std::cout << "Unknown data type in datatype option" << std::endl;
        return false;
    }

//    if (do_checks && datatype != MPI_INT) {
//        // FIXME get rid of cout some way!
//        std::cout << "Only int data type is supported with check option" << std::endl;
//        return false;
//    }

    num_threads = 1;
    if (mode_multiple) {
#pragma omp parallel default(shared)
#pragma omp master
        num_threads = omp_get_num_threads();
    }
//    input = (thread_local_data_t *)malloc(sizeof(thread_local_data_t) * num_threads);
    input.resize(num_threads);    
    for (int thread_num = 0; thread_num < num_threads; thread_num++) {
        input[thread_num].comm = duplicate_comm(mode_multiple, thread_num);
        input[thread_num].warmup = parser.get_result<int>("warmup");
        input[thread_num].repeat = parser.get_result<int>("repeat");
    }

    std::vector<int> topo;
    parser.get_result_vec<int>("topo", topo);

    // -- HALO specific part
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    fill_in(topo);

    if (required_nranks > nranks && rank == 0) {
        // FIXME get rid of cout some way!
        std::cout << "Not enough ranks, " << required_nranks << " min. required" << std::endl;
        return false;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    prepared = true;
    return true;
}

#define HANDLE_PARAMETER(TYPE, NAME) if (key == #NAME) { \
                                        result = smart_ptr< TYPE >(&NAME); \
                                        result.detach_ptr(); }


template<> any BenchmarkSuite<BS_GENERIC>::get_parameter(const std::string &key) {
    using namespace NS_HALO;
    assert(prepared);
    any result;
    HANDLE_PARAMETER(std::vector<thread_local_data_t>, input);
    HANDLE_PARAMETER(int, num_threads);   
    HANDLE_PARAMETER(int, mode_multiple);
    HANDLE_PARAMETER(int, malloc_align);
    HANDLE_PARAMETER(MPI_Datatype, datatype);
    HANDLE_PARAMETER(int, ndims);
    HANDLE_PARAMETER(int, required_nranks);
    HANDLE_PARAMETER(std::vector<int>, ranksperdim);
    HANDLE_PARAMETER(std::vector<int>, mults);
    HANDLE_PARAMETER(int, rank);
    HANDLE_PARAMETER(int, nranks);
    HANDLE_PARAMETER(std::vector<int>, count);
    return result;
}

}

