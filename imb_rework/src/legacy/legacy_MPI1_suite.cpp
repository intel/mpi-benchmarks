#ifdef MPI1

#include <set>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <mpi.h>
#include "args_parser.h"
#include "benchmark_suites_collection.h"
#include "utils.h"
#include "any.h"
#include "benchmark_suite.h"

extern "C" {
#include "IMB_benchmark.h"
#include "IMB_comm_info.h"
#include "IMB_prototypes.h"
}

#include "reworked_IMB_functions.h"

using namespace std;

DECLARE_BENCHMARK_SUITE_STUFF(BS_MPI1, IMB-MPI1)

namespace NS_MPI1 {
    struct comm_info c_info;
    struct iter_schedule ITERATIONS;
    struct LEGACY_GLOBALS glob;
    bool prepared = false;
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

template <> bool BenchmarkSuite<BS_MPI1>::declare_args(args_parser &parser, std::ostream &output) const {
    UNUSED(output);
    parser.set_current_group(get_name());
    parser.add<int>("npmin", 2).set_caption("NPmin").
        set_description(
            "The argument after npmin is NPmin,\n"
            "the minimum number of processes to run on\n"
            "(then if IMB is started on NP processes, the process numbers\n"
            "NPmin, 2*NPmin, ... ,2^k * NPmin < NP, NP are used)\n"
            "To run on just NP processes, run IMB on NP and select -npmin NP\n"
            "\n"
            "Default:\n"
            "NPmin=2\n");
    parser.add<int>("multi", -1).set_caption("MultiMode").
        set_description(
            "The argument after -multi is MultiMode (0 or 1)\n"
            "\n"
            "If -multi is selected, running the N process version of a benchmark\n"
            "on NP overall, means running on (NP/N) simultaneous groups of N each.\n"
            "\n"
            "MultiMode only controls default (0) or extensive (1) output charts.\n"
            "0: only lowest performance groups is output\n"
            "1: all groups are output\n"
            "\n"
            "Default:\n"
            "multi off\n");
    parser.add_vector<float>("off_cache", "-1.0,0.0", ',', 1, 2).
           set_caption("cache_size[,cache_line_size]").
           set_mode(args_parser::option::APPLY_DEFAULTS_ONLY_WHEN_MISSING).
           set_description(
                "the argument after off_cache can be either 1 single number (cache_size),\n"
                "or 2 comma separated numbers (cache_size,cache_line_size), or just -1\n"
                "\n"
                "By default, without this flag, the communications buffer is\n"
                "the same within all repetitions of one message size sample;\n"
                "most likely, cache reusage is yielded and thus throughput results\n"
                "that might be non realistic.\n"
                "\n"
                "With -off_cache, it is attempted to avoid cache reusage.\n"
                "cache_size is a float for an upper bound of the size of the last level cache in MBytes\n"
                "cache_line_size is assumed to be the size (Bytes) of a last level cache line\n"
                "(can be an upper estimate).\n"
                "The sent/recv'd data are stored in buffers of size ~ 2 x MAX( cache_size, message_size );\n"
                "when repetitively using messages of a particular size, their addresses are advanced within those\n"
                "buffers so that a single message is at least 2 cache lines after the end of the previous message.\n"
                "Only when those buffers have been marched through (eventually), they will re-used from the beginning.\n"
                "\n"
                "A cache_size and a cache_line_size are assumed as statically defined\n"
                "in  => IMB_mem_info.h; these are used when -off_cache -1 is entered\n"
                "\n"
                "remark: -off_cache is effective for IMB-MPI1, IMB-EXT, but not IMB-IO\n"
                "\n"
                "Examples:\n"
                "-off_cache -1 (use defaults of IMB_mem_info.h);\n"
                "-off_cache 2.5 (2.5 MB last level cache, default line size);\n"
                "-off_cache 16,128 (16 MB last level cache, line size 128);\n"
                "\n"
                "NOTE: the off_cache mode might also be influenced by eventual internal\n"
                "caching with the MPI library. This could make the interpretation\n"
                "intricate.\n"
                "\n"
                "Default:\n"
                "no cache control, data likely to come out of cache most of the time\n");
    parser.add_vector<int>("iter", "1000,40,100", ',', 1, 3).
           set_caption("msgspersample[,overall_vol[,msgs_nonaggr]]").
           set_description(
                "The argument after -iter can contain from 1 to 3 comma separated values\n"
                "3 integer numbers override the defaults\n"
                "MSGSPERSAMPLE, OVERALL_VOL, MSGS_NONAGGR of IMB_settings.h\n"
                "Examples:\n"
                "-iter 2000        (override MSGSPERSAMPLE by value 2000)\n"
                "-iter 1000,100    (override OVERALL_VOL by 100)\n"
                "-iter 1000,40,150 (override MSGS_NONAGGR by 150)\n"
                "\n"   
                "Default:\n"
                "iteration control through parameters MSGSPERSAMPLE,OVERALL_VOL,MSGS_NONAGGR => IMB_settings.h\n");
    parser.add<string>("iter_policy", "dynamic").set_caption("iter_policy").
           set_description(
                "The argument after -iter_policy is a one from possible strings,\n"
                "Specifying that policy will be used for auto iteration control:\n"
                "dynamic,multiple_np,auto,off\n"
                "\n"
                "Example:\n"
                "-iter_policy auto\n"
                "\n"
                "Default:\n"
                "dynamic\n");
    parser.add<float>("time", 10.0f).set_caption("max_runtime per sample").
           set_description(
                "The argument after -time is a float, specifying that\n"
                "a benchmark will run at most that many seconds per message size\n"
                "the combination with the -iter flag or its defaults is so that always\n"
                "the maximum number of repetitions is chosen that fulfills all restrictions\n"
                "\n"
                "Example:\n"
                "-time 0.150       (a benchmark will (roughly) run at most 150 milli seconds per message size, if\n"
                "the default (or -iter selected) number of repetitions would take longer than that)\n"
                "\n"
                "remark: per sample, the rough number of repetitions to fulfill the -time request\n"
                "is estimated in preparatory runs that use ~ 1 second overhead\n"
                "\n"
                "Default:\n"
                "A fixed time limit SECS_PER_SAMPLE =>IMB_settings.h; currently set to 10\n" 
                "(new default in IMB_3.2)\n");
    parser.add<float>("mem", 1.0f).
           set_caption("max. per process memory for overall message buffers").
           set_description(
               "The argument after -mem is a float, specifying that\n"
               "at most that many GBytes are allocated per process for the message buffers\n"
               "if the size is exceeded, a warning will be output, stating how much memory\n"
               "would have been necessary, but the overall run is not interrupted\n"
               "\n"
               "Example:\n"
               "-mem 0.2         (restrict memory for message buffers to 200 MBytes per process)\n"
               "\n"
               "Default:\n"
               "the memory is restricted by MAX_MEM_USAGE => IMB_mem_info.h\n");
    parser.add<string>("msglen", "").set_caption("Lengths_file").
           set_description(
               "The argument after -msglen is a lengths_file, an ASCII file, containing any set of nonnegative\n"
               "message lengths, 1 per line\n"
               "\n"
               "Default:\n"
               "no lengths_file, lengths defined by settings.h, settings_io.h\n");
    parser.add_vector<int>("map", "1x1", 'x', 2, 2).set_caption("PxQ").
           set_description(
               "The argument after -map is PxQ, P,Q are integer numbers with P*Q <= NP\n"
               "enter PxQ with the 2 numbers separated by letter \"x\" and no blancs\n"
               "the basic communicator is set up as P by Q process grid\n"
               "\n"
               "If, e.g., one runs on N nodes of X processors each, and inserts\n"
               "P=X, Q=N, then the numbering of processes is \"inter node first\"\n"
               "running PingPong with P=X, Q=2 would measure inter-node performance\n"
               "(assuming MPI default would apply 'normal' mapping, i.e. fill nodes\n"
               "first priority)\n"
               "\n"   
               "Default:\n"
               "Q=1\n");
    parser.add_vector<int>("msglog", "0:22", ':', 1, 2).
           set_caption("min_msglog:max_msglog").
           set_mode(args_parser::option::APPLY_DEFAULTS_ONLY_WHEN_MISSING).
           set_description(
                "The argument after -msglog min:max, min and max are positive integer numbers, min<max\n"
                "where min is power of 2 so that second smallest data transfer size is max(unit,2^min)\n"
                "(the smallest always being 0), where unit = sizeof(float) for reductions, unit = 1 else\n"
                "max is power of 2 so that 2^max is largest messages size, max must be less than 31\n");
    parser.add<bool>("root_shift", false).set_caption("on or off").
           set_description(
               "Controls root change at each iteration step for certain collective benchmarks,\n"
               "possible argument values are on (1|enable|yes) or off (0|disable|no)\n"
               "\n"
               "Default:\n"
               "off\n");
    parser.add<bool>("sync", true).set_caption("on or off").
           set_description(
               "Controls whether all processes are syncronized at each iteration step in collective benchmarks,\n"
               "possible argument values are on (1|enable|yes) or off (0|disable|no)"
               "\n"
               "Default:\n"
               "on\n");
    parser.add<bool>("imb_barrier", false).set_caption("on or off").
           set_description(
               "Use internal MPI-independent barrier syncronization implementation,\n"
               "possible argument values are on (1|enable|yes) or off (0|disable|no)\n"
               "\n"
               "Default:\n"
               "off\n");                   
    parser.set_default_current_group();
    return true;
}

#define BASIC_INPUT_EXPERIMENT 1

template <typename T>
void preprocess_list(T &list) {
    T tmp;
    transform(list.begin(), list.end(), inserter(tmp, tmp.end()), tolower);
    list = tmp;
}

template <> bool BenchmarkSuite<BS_MPI1>::prepare(const args_parser &parser, const vector<string> &benchs, std::ostream &output) {
    using namespace NS_MPI1;
    vector<string> all_benchs, spare_benchs = benchs, intersection = benchs;
    BenchmarkSuite<BS_MPI1>::get_full_list(all_benchs);
    set_operations::exclude(spare_benchs, all_benchs);
    set_operations::exclude(intersection, spare_benchs);
    if (intersection.size() == 0)
        return true;

    prepared = true;

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
    glob.NP_min = parser.get<int>("npmin");
    if (glob.NP_min <= 0) {
        cmd_line_error = true;
    }

    // multi
    c_info.group_mode = parser.get<int>("multi");

    // off_cache
    vector<float> csize;
    parser.get<float>("off_cache", csize);
    if (csize.size() == 1) {
        ITERATIONS.cache_size = csize[0];
        ITERATIONS.cache_line_size = CACHE_LINE_SIZE;
        if (ITERATIONS.cache_size < 0.0) {
            ITERATIONS.cache_size = CACHE_SIZE;
        }
    } else {
        assert(csize.size() == 2);
        ITERATIONS.cache_size = csize[0];
        ITERATIONS.cache_line_size = (int)csize[1];
        if (csize[1] != floor(csize[1])) {
            cmd_line_error = true;
        }
    }
    if (ITERATIONS.cache_size > 0.0)
        ITERATIONS.off_cache = 1;

    // iter
    vector<int> given_iter;
    parser.get<int>("iter", given_iter);
    ITERATIONS.msgspersample = given_iter[0];
    ITERATIONS.overall_vol = given_iter[1] * 1024 * 1024;
    ITERATIONS.msgs_nonaggr = given_iter[2];

    // iter_policy
    string given_iter_policy = parser.get<string>("iter_policy");
    if (given_iter_policy == "dynamic") { ITERATIONS.iter_policy = imode_dynamic; }
    if (given_iter_policy == "off") { ITERATIONS.iter_policy = imode_off; }
    if (given_iter_policy == "multiple_np") { ITERATIONS.iter_policy = imode_multiple_np; }
    if (given_iter_policy == "auto") { ITERATIONS.iter_policy = imode_auto; }

    // time
    ITERATIONS.secs = parser.get<float>("time");

    // mem
    c_info.max_mem = parser.get<float>("mem");

    // map
    vector<int> given_map;
    parser.get<int>("map", given_map);
    c_info.px = given_map[0];
    c_info.py = given_map[1];
    if (c_info.px * c_info.py > c_info.w_num_procs) {
        cmd_line_error = true;
    }

    // msglen
    string given_msglen_filename = parser.get<string>("msglen");
    if (given_msglen_filename != "") {
        if (!load_msg_sizes(given_msglen_filename.c_str())) {
            output << "Sizes File " << given_msglen_filename << " invalid or doesnt exist" << endl;
            cmd_line_error = true;
        }
    }

    // msglog
    vector<int> given_msglog;
    parser.get<int>("msglog", given_msglog);
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
    c_info.root_shift = (parser.get<bool>("root_shift") ? 1 : 0);

    // sync
    c_info.sync = (parser.get<bool>("sync") ? 1 : 0);

    // imb_barrier
    IMB_internal_barrier = (parser.get<bool>("imb_barrier") ? 1 : 0);

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
        string cmd_line;
        parser.get_command_line(cmd_line);
        fprintf(unit, "# %s \n\n", cmd_line.c_str());
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
        for (vector<string>::iterator it = intersection.begin(); it != intersection.end(); ++it) {
            printf("# %s\n", it->c_str());
        }
    }
    return true;
}

template <> void BenchmarkSuite<BS_MPI1>::finalize(const vector<string> &benchs,
                                                   std::ostream &output) {
    UNUSED(output);
    using namespace NS_MPI1;
    if (!prepared)
        return;
    for (vector<string>::const_iterator it = benchs.begin(); it != benchs.end(); ++it) {
        smart_ptr<Benchmark> b = get_instance().create(*it);
        if (b.get() == NULL) 
            continue;
        // do nothing
    }
    if (c_info.w_rank == 0) {
        fprintf(unit,"\n\n# All processes entering MPI_Finalize\n\n");
    }
}

template <> void BenchmarkSuite<BS_MPI1>::get_bench_list(set<string> &benchs, 
                                                         BenchmarkSuiteBase::BenchListFilter filter) const {
    BenchmarkSuite<BS_MPI1>::get_full_list(benchs);
    if (filter == BenchmarkSuiteBase::DEFAULT_BENCHMARKS) {
        for (set<string>::iterator it = benchs.begin(); it != benchs.end(); ++it) {
            smart_ptr<Benchmark> b = get_instance().create(*it);
            if (b.get() == NULL)            
                continue;
            if (!b->is_default()) 
                benchs.erase(it);
        }
    }
}

template <> void BenchmarkSuite<BS_MPI1>::get_bench_list(vector<string> &benchs, 
                                                         BenchmarkSuiteBase::BenchListFilter filter) const {
    BenchmarkSuite<BS_MPI1>::get_full_list(benchs);
    if (benchs.size() == 0)
        return;
    if (filter == BenchmarkSuiteBase::DEFAULT_BENCHMARKS) {
        for (size_t i = benchs.size() - 1; i != 0; i--) {
            smart_ptr<Benchmark> b = get_instance().create(benchs[i]);
            if (b.get() == NULL) {
                continue;
            }
            if (!b->is_default()) 
                benchs.erase(benchs.begin() + i);
        }
    }
}

#define HANDLE_PARAMETER(TYPE, NAME) if (key == #NAME) { \
                                        result = smart_ptr< TYPE >(&NAME); \
                                        result.detach_ptr(); }


template<> any BenchmarkSuite<BS_MPI1>::get_parameter(const std::string &key) {
    using namespace NS_MPI1;
    any result;
    HANDLE_PARAMETER(comm_info, c_info);
    HANDLE_PARAMETER(iter_schedule, ITERATIONS);
    HANDLE_PARAMETER(LEGACY_GLOBALS, glob);
    return result;
}

#endif

