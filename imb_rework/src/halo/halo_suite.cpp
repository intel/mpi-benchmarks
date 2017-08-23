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

static MPI_Comm duplicate_comm(int mode_multiple, int thread_num)
{
    UNUSED(thread_num);
    MPI_Comm comm =  MPI_COMM_WORLD, new_comm;
    if(mode_multiple) {
        MPI_Comm_dup(comm, &new_comm);
        return new_comm;
    }
    return comm;
}

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

using namespace std;

namespace NS_HALO {
    vector<thread_local_data_t> input;
    int mode_multiple;
    int stride;
    int num_threads;
    int rank, nranks;
    bool prepared = false;
    vector<int> cnt;
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
    parser.add_option_with_defaults<string>("datatype", "int").
        set_caption("int|char");

    parser.add_option_with_defaults_vec<int>("topo", "1", '.');
}

template <> bool BenchmarkSuite<BS_GENERIC>::prepare(const args_parser &parser, const set<string> &) {
    using namespace NS_HALO;

    parser.get_result_vec<int>("count", cnt);
    mode_multiple = (parser.get_result<string>("thread_level") == "multiple");
    stride = parser.get_result<int>("stride");

    malloc_align = parser.get_result<int>("malloc_align");

//    do_checks = parser.get_result<bool>("check");

    string dt = parser.get_result<string>("datatype");
    if (dt == "int") datatype = MPI_INT;
    else if (dt == "char") datatype = MPI_CHAR;
    else {
        // FIXME get rid of cout some way!
        cout << "Unknown data type in datatype option" << endl;
        return false;
    }

//    if (do_checks && datatype != MPI_INT) {
//        // FIXME get rid of cout some way!
//        cout << "Only int data type is supported with check option" << endl;
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
        cout << "Not enough ranks, " << required_nranks << " min. required" << endl;
        return false;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    prepared = true;
    return true;
}

template<> any BenchmarkSuite<BS_GENERIC>::get_parameter(const string &key) {
    using namespace NS_HALO;
    assert(prepared);
    any result;
    if (key == "input") { result = smart_ptr<vector<thread_local_data_t> >(&input); result.detach_ptr(); }
    if (key == "num_threads") { result = smart_ptr<int>(&num_threads); result.detach_ptr(); }
    if (key == "mode_multiple") { result = smart_ptr<int>(&mode_multiple); result.detach_ptr(); }
    if (key == "malloc_align") { result = smart_ptr<int>(&malloc_align); result.detach_ptr(); }
    if (key == "datatype") { result = smart_ptr<MPI_Datatype>(&datatype); result.detach_ptr(); }

    if (key == "ndims") { result = smart_ptr<int>(&ndims); result.detach_ptr(); }
    if (key == "required_nranks") { result = smart_ptr<int>(&required_nranks); result.detach_ptr(); }
    if (key == "ranksperdim") { result = smart_ptr<vector<int> >(&ranksperdim); result.detach_ptr(); }
    if (key == "mults") { result = smart_ptr<vector<int> >(&mults); result.detach_ptr(); }
    if (key == "rank") { result = smart_ptr<int>(&rank); result.detach_ptr(); }
    if (key == "nranks") { result = smart_ptr<int>(&nranks); result.detach_ptr(); }
    if (key == "count") { result = smart_ptr<vector<int> >(&cnt); result.detach_ptr(); }
    return result;
}

}

