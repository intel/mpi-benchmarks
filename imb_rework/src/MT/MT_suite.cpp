#include <mpi.h>
#include <omp.h>
#include <set>
#include <vector>
#include <string>
#include <map>
#include "benchmark.h"
#include "benchmark_suite.h"
#include "utils.h"

#include "MT_suite.h"

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

using namespace std;

namespace NS_MT {
    immb_local_t *input;
    int mode_multiple;
    int stride;
    int num_threads;
    int rank;
    bool prepared = false;
    vector<int> cnt;
    vector<string> comm_opts;
    int malloc_align;
    malopt_t malloc_option;
    barropt_t barrier_option;
    bool do_checks;
    MPI_Datatype datatype;
};

map<string, const Benchmark*, set_operations::case_insens_cmp> *BenchmarkSuite<BS_MT>::pnames = 0;
BenchmarkSuite<BS_MT> *BenchmarkSuite<BS_MT>::instance = 0;

template <> const std::string BenchmarkSuite<BS_MT>::get_name() const { return "IMB-MT"; }

template <> void BenchmarkSuite<BS_MT>::declare_args(args_parser &parser) const {
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
}

template <> bool BenchmarkSuite<BS_MT>::prepare(const args_parser &parser, const set<string> &benchs) {
    using namespace NS_MT;
    parser.get_result_vec<string>("comm", comm_opts);
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
            size_t n = comm_opts.size();
            _ARRAY_ALLOC(input[thread_num].comm, MPI_Comm, n);
            for (size_t i = 0; i < n; i++) {
                input[thread_num].comm[i] = immb_convert_comm(comm_opts[i].c_str(), mode_multiple, thread_num);
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

    if (rank == 0) {
        cout << "#------------------------------------------------------------" << endl;
        cout << "#    Intel (R) MPI Benchmarks " << "PREVIEW" << ", MT part    " << endl;
        cout << "#------------------------------------------------------------" << endl;
        cout << "#" << endl;
        cout << "# ******* WARNING! THIS IS PREVIEW VERSION!      *******" << endl;
        cout << "# ******* FOR PRELIMINARY OVERVIEW ONLY!         *******" << endl;
        cout << "# ******* DON'T USE FOR ANY ACTUAL BENCHMARKING! *******" << endl;
        cout << "#" << endl;
        cout << "#" << endl;
    }

    return true;
}

template <> void BenchmarkSuite<BS_MT>::finalize(const set<string> &benchs) {
    using namespace NS_MT;
    for (int thread_num = 0; thread_num < num_threads; thread_num++) {
        _ARRAY_FREE(input[thread_num].comm);
        _ARRAY_FREE(input[thread_num].count);
    }
    if (prepared && rank == 0)
        cout << endl;
}


void *MTBenchmarkSuite::get_internal_data_ptr(const std::string &key, int i) {
    using namespace NS_MT;
    if (key == "input[thread_num]") return &input[i];
    if (key == "num_threads") return &num_threads;
    if (key == "mode_multiple") return &mode_multiple;
    if (key == "stride") return &stride;
    if (key == "malloc_align") return &malloc_align;
    if (key == "malloc_option") return &malloc_option;
    if (key == "barrier_option") return &barrier_option;
    if (key == "do_checks") return &do_checks;
    if (key == "datatype") return &datatype;
    assert(false);
    return 0;
}

