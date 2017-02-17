#include <set>
#include <vector>
#include <string>
#include <algorithm>
#include <mpi.h>
#include "benchmark_suite_MPI1.h"
#include "IMB_comm_info.h"

#include "reworked_IMB_functions.h"
#include "utils.h"

using namespace std;

//#ifdef MPI1
map<string, const Benchmark*, case_insens_cmp> *BenchmarkSuite<BS_MPI1>::pnames = 0;
BenchmarkSuite<BS_MPI1> *BenchmarkSuite<BS_MPI1>::instance = 0;
//#endif

namespace NS_MPI1 {
    struct comm_info c_info;
    struct iter_schedule ITERATIONS;
    struct LEGACY_GLOBALS glob;
}

bool load_msg_sizes(const char *filename)
{
    using namespace NS_MPI1;

    FILE*t = fopen(filename, "r");
    if (t == NULL)
        return false;

    int n_lens = 0;
    char inp_line[72];
    while(fgets(inp_line,72,t)) {
        if( inp_line[0] != '#' && strlen(inp_line)>1 )
            n_lens++;
    }
    fclose(t);

    if (n_lens == 0)
        return false;

    t = fopen(filename, "r");
    if (t == NULL)
        return false;

    c_info.n_lens = n_lens;

    char S[32];
    int sz, isz;

    c_info.msglen = (int *)malloc(n_lens * sizeof(int));

    isz=-1;

    while(fgets(inp_line,72,t)) {
        S[0]='\0';
        if( inp_line[0] != '#' && strlen(inp_line)-1 ) {
            int ierr;
            sz=0;

            ierr=sscanf(&inp_line[0],"%d%s",&sz,&S[0]);
            if( ierr<=0 || ierr==EOF || sz<0 ) {
                ierr=-1;
            } else if(ierr==2) {
                if      (S[0]=='k' ||  S[0]=='K') {sz=sz*1024;}
                else if (S[0]=='m' ||  S[0]=='M') {sz=sz*1024*1024;}
                else {
                    ierr=-1;
                }
            } /*else if(ierr==2) */

            if( ierr>0 ) {
                isz++;
                c_info.msglen[isz]=sz;
            } else {
                return false;
            }
        } /*if( inp_line[0] != '#' && strlen(inp_line)-1 )*/
    } /*while(fgets(inp_line,72,t))*/

    n_lens = c_info.n_lens = isz + 1;
    fclose(t);

    if (n_lens==0)
        return false;
    return true;
}

template <> void BenchmarkSuite<BS_MPI1>::init() {
    set<string> benchs;
    BenchmarkSuite<BS_MPI1>::get_full_list(benchs);
    for (set<string>::iterator it = benchs.begin(); it != benchs.end(); ++it) {
        smart_ptr<Benchmark> b = get_instance().create(*it);
        if (!b->init_description())
            throw logic_error("BenchmarkSuite<BS_MPI1>: wrong description of one of benchmarks in suite");
    }
}

template <> void BenchmarkSuite<BS_MPI1>::declare_args(args_parser &parser) const {
    parser.add_option_with_defaults<int>("npmin", 2).set_caption("NPmin");
    parser.add_option_with_defaults<int>("multi", -1).set_caption("MultiMode");
    parser.add_option_with_defaults_vec<float>("off_cache", "-1.0,0.0", ',', 1, 2).
        set_caption("cache_size[,cache_line_size]").
        set_mode(args_parser::option::APPLY_DEFAULTS_ONLY_WHEN_MISSING);
    parser.add_option_with_defaults_vec<int>("iter", "1000,40,100", ',', 1, 3).
        set_caption("msgspersample[,overall_vol[,msgs_nonaggr]]");
    parser.add_option_with_defaults<string>("iter_policy", "dynamic").
        set_caption("iter_policy");
    parser.add_option_with_defaults<float>("time", 10.0f).set_caption("max_runtime per sample");
    parser.add_option_with_defaults<float>("mem", 1.0f).
        set_caption("max. per process memory for overall message buffers");
    parser.add_option_with_defaults<string>("msglen", "").set_caption("Lengths_file");
    parser.add_option_with_defaults_vec<int>("map", "1x1", 'x', 2, 2).set_caption("PxQ");
    parser.add_option_with_defaults_vec<int>("msglog", "0:22", ':', 1, 2).
        set_caption("min_msglog:max_msglog").
        set_mode(args_parser::option::APPLY_DEFAULTS_ONLY_WHEN_MISSING);
    parser.add_option_with_defaults<bool>("root_shift", false).set_caption("on or off");
    parser.add_option_with_defaults<bool>("sync", true).set_caption("on or off");
    parser.add_option_with_defaults<bool>("imb_barrier", false).set_caption("on or off");
}

#define BASIC_INPUT_EXPERIMENT 1

template <typename T>
void preprocess_list(T &list) {
    T tmp;
    transform(list.begin(), list.end(), inserter(tmp, tmp.end()), to_lower);
    list = tmp;
}

template <> bool BenchmarkSuite<BS_MPI1>::prepare(const args_parser &parser, const set<string> &benchs) {
    using namespace NS_MPI1;
    set<string> all_benchs, intersection;
    BenchmarkSuite<BS_MPI1>::get_full_list(all_benchs);
    set_operations::preprocess_list(all_benchs);
    set_intersection(all_benchs.begin(), all_benchs.end(), benchs.begin(), benchs.end(),
                     inserter(intersection, intersection.begin()));
    if (intersection.size() == 0)
        return true;
 
    IMB_set_default(&c_info);
    IMB_init_pointers(&c_info);

#if BASIC_INPUT_EXPERIMENT == 1
    {
        /* run time control as default */
        ITERATIONS.n_sample=0;
        ITERATIONS.off_cache=0;
        ITERATIONS.cache_size=-1;
        ITERATIONS.s_offs = ITERATIONS.r_offs = 0;
        ITERATIONS.s_cache_iter = ITERATIONS.r_cache_iter = 1;
        ITERATIONS.msgspersample=MSGSPERSAMPLE;
        ITERATIONS.msgs_nonaggr=MSGS_NONAGGR;
        ITERATIONS.overall_vol=OVERALL_VOL;
        ITERATIONS.secs=SECS_PER_SAMPLE;
        ITERATIONS.iter_policy=ITER_POLICY;
        ITERATIONS.numiters=(int*)NULL;

        MPI_Comm_rank(MPI_COMM_WORLD,&c_info.w_rank);
        MPI_Comm_size(MPI_COMM_WORLD,&c_info.w_num_procs);

        unit = stdout;

        if( c_info.w_rank == 0 && strlen(OUTPUT_FILENAME) > 0 )
            unit = fopen(OUTPUT_FILENAME,"w");

        c_info.group_mode = -1;
        glob.NP_min=2;
    }  
    bool cmd_line_error = false;

    // npmin
    glob.NP_min = parser.get_result<int>("npmin");
    if (glob.NP_min <= 0) {
        cmd_line_error = true;
    }

    // multi
    c_info.group_mode = parser.get_result<int>("multi");

    // off_cache
    vector<float> csize;
    parser.get_result_vec<float>("off_cache", csize);
    if (csize.size() == 1) {
        ITERATIONS.cache_size = csize[0];
        ITERATIONS.cache_line_size = CACHE_LINE_SIZE;
        if (ITERATIONS.cache_size < 0.0) {
            ITERATIONS.cache_size = CACHE_SIZE;
        }
    } else {
        ITERATIONS.cache_size = csize[0];
        ITERATIONS.cache_line_size = (int)csize[1];
        if (csize[1] != floor(csize[1])) {
            cmd_line_error = true;
        }
    }

    // iter
    vector<int> given_iter;
    parser.get_result_vec<int>("iter", given_iter);
    ITERATIONS.msgspersample = given_iter[0];
    ITERATIONS.overall_vol = given_iter[1] * 1024 * 1024;
    ITERATIONS.msgs_nonaggr = given_iter[2];

    // iter_policy
    string given_iter_policy = parser.get_result<string>("iter_policy");
    if (given_iter_policy == "dynamic") { ITERATIONS.iter_policy = imode_dynamic; }
    if (given_iter_policy == "off") { ITERATIONS.iter_policy = imode_off; }
    if (given_iter_policy == "multiple_np") { ITERATIONS.iter_policy = imode_multiple_np; }
    if (given_iter_policy == "auto") { ITERATIONS.iter_policy = imode_auto; }

    // time
    ITERATIONS.secs = parser.get_result<float>("time");

    // mem
    c_info.max_mem = parser.get_result<float>("mem");

    // map
    vector<int> given_map;
    parser.get_result_vec<int>("map", given_map);
    c_info.px = given_map[0];
    c_info.py = given_map[1];
    if (c_info.px * c_info.py > c_info.w_num_procs) {
        cmd_line_error = true;
    }

    // msglen
    string given_msglen_filename = parser.get_result<string>("msglen");
    if (given_msglen_filename != "") {
        if (!load_msg_sizes(given_msglen_filename.c_str())) {
            cout << "Sizes File " << given_msglen_filename << " invalid or doesnt exist" << endl;
            cmd_line_error = true;
        }
    }

    // msglog
    vector<int> given_msglog;
    parser.get_result_vec<int>("msglog", given_msglog);
    if (given_msglog.size() == 1) {
        c_info.max_msg_log = c_info.min_msg_log = given_msglog[0];
    } else {
        c_info.min_msg_log = given_msglog[0];
        c_info.max_msg_log = given_msglog[1];
    }
    const int MAX_INT_LOG = 31;
    if (c_info.min_msg_log < 0 || c_info.min_msg_log >= MAX_INT_LOG)
        cmd_line_error = true;
    if (c_info.max_msg_log < 0 || c_info.max_msg_log >= MAX_INT_LOG)
        cmd_line_error = true;
    if (c_info.max_msg_log < c_info.min_msg_log)
        cmd_line_error = true;
    
    // root_shift
    c_info.root_shift = (parser.get_result<bool>("root_shift") ? 1 : 0);

    // sync
    c_info.sync = (parser.get_result<bool>("sync") ? 1 : 0);

    // imb_barrier
    IMB_internal_barrier = (parser.get_result<bool>("imb_barrier") ? 1 : 0);

    if (cmd_line_error)
        return false;

    if(ITERATIONS.iter_policy != imode_off && ITERATIONS.iter_policy != imode_invalid) {
        ITERATIONS.numiters = (int *)malloc(c_info.n_lens * sizeof(int));
    }

#endif
    
#if BASIC_INPUT_EXPERIMENT == 0
    struct Bench *BList;
    char *argv[] = { "" };
    int argc = 0;
    IMB_basic_input(&c_info, &BList, &ITERATIONS, &argc, (char ***)argv, &glob.NP_min);
#endif    

    if (c_info.w_rank == 0 ) {
        IMB_general_info();
        fprintf(unit,"\n\n# Calling sequence was: \n\n");
        fprintf(unit,"# ------------------- \n\n");
        if (c_info.n_lens) {
            fprintf(unit,"# Message lengths were user defined\n");
        } else {
            fprintf(unit,"# Minimum message length in bytes:   %d\n",0);
            fprintf(unit,"# Maximum message length in bytes:   %d\n", 1<<c_info.max_msg_log);
        }

        fprintf(unit,"#\n");
        fprintf(unit,"# MPI_Datatype                   :   MPI_BYTE \n");
        fprintf(unit,"# MPI_Datatype for reductions    :   MPI_FLOAT\n");
        fprintf(unit,"# MPI_Op                         :   MPI_SUM  \n");
        fprintf(unit,"# \n");
        fprintf(unit,"# \n");
        fprintf(unit,"\n");
        fprintf(unit,"# List of Benchmarks to run:\n\n");
        for (set<string>::iterator it = intersection.begin(); it != intersection.end(); ++it) {
            printf("# %s\n", it->c_str());
        }
    }
    return true;
}

template <> void BenchmarkSuite<BS_MPI1>::get_bench_list(set<string> &benchs, 
                                                         BenchmarkSuiteBase::BenchListFilter filter) const {
    BenchmarkSuite<BS_MPI1>::get_full_list(benchs);
    if (filter == BenchmarkSuiteBase::DEFAULT_BENCHMARKS) {
        for (set<string>::iterator it = benchs.begin(); it != benchs.end();) {
            smart_ptr<Benchmark> b = get_instance().create(*it);
            if (!b->is_default()) {
                benchs.erase(it++);
            }
            else
                ++it;
        }
    }
}

void *OriginalBenchmarkSuite_MPI1::get_internal_data_ptr(const std::string &key) {
    using namespace NS_MPI1;
    if (key == "c_info") return &c_info;
    if (key == "ITERATIONS") return &ITERATIONS;
    if (key == "glob") return &glob;
    return 0;
}

