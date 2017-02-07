#include "args_parser.h"
#include "smart_ptr.h"
#include <stdexcept>
#include <fstream>

#include "benchmark_suite.h"
#include "benchmark_suite_MPI1.h"

#include <mpi.h>

using namespace std;

extern void check_parser();

int main(int argc, char **argv)
{
#if 0
    check_parser();
#endif    

    MPI_Init(&argc, &argv);
    try {
        //args_parser parser(argc, argv, "/", ':');
        //args_parser parser(argc, argv, "--", '=');
        args_parser parser(argc, argv, "-", ' ');

        parser.add_option_with_defaults<string>("input", "").
            set_caption("input", "filename");
        parser.add_option_with_defaults_vec<string>("include").
            set_caption("include", "benchmark[,benchmark,[...]");
        parser.add_option_with_defaults_vec<string>("exclude").
            set_caption("exclude", "benchmark[,benchmark,[...]");

        // extra non-option arguments 
        parser.set_current_group("EXTRA_ARGS");
        parser.add_option_with_defaults_vec<string>("(benchmarks)").
            set_caption("(benchmarks)", "benchmark[,benchmark,[...]]"); 
        parser.set_default_current_group();

        // bechmark suite related args
        OriginalBenchmarkSuite_MPI1::declare_args(parser);
        //BenchmarkSuite<BS_OSU>::declare_args(parser);

        // "system" option args to do special things, not dumped to files
        parser.set_current_group("SYS");
        parser.add_option_with_defaults<string>("dump", "").
            set_caption("dump", "config.yaml");
        parser.add_option_with_defaults<string>("load", "").
            set_caption("load", "config.yaml");
        parser.set_default_current_group();
         
        if (!parser.parse()) {
            return 1;
        }
        string infile;  
        infile = parser.get_result<string>("load");
        if (infile != "") {
            ifstream in(infile.c_str(), ios_base::in);
            parser.load(in);
            if (!parser.parse()) {
                throw logic_error("parser after load failed");
            }
        }
        string outfile;  
        outfile = parser.get_result<string>("dump");
        if (outfile != "") {
            string out;
            out = parser.dump();
            ofstream of(outfile.c_str(), ios_base::out);
            of << out;
        }
        
        vector<string> requested_benchmarks, to_include, to_exclude;
        parser.get_result_vec<string>("(benchmarks)", requested_benchmarks);
        parser.get_result_vec<string>("include", to_include);
        parser.get_result_vec<string>("exclude", to_exclude);

        vector<string> default_benchmarks, all_benchmarks;
        vector<string> actual_benchmark_list;
#if 0            
#ifdef MPI1
        BenchmarkSuite<BS_MPI1>::get_default_list(default_benchmarks);
        BenchmarkSuite<BS_MPI1>::get_full_list(all_benchmarks);
#endif            

        if (to_include.size() != 0 || to_exclude.size() != 0) {
            if (requested_benchmarks.size() != 0) {
                // FIXME we can actually work it out
                cout << "ERROR: can't combine -include and -exclude options with explicit benchmark list" << endl;
                return 1;
            } else {
                combine(actual_bechmark_list, default_benchmarks);
                combine(actual_bechmark_list, to_include);
                exclude(actual_bechmark_list, to_exclude);
           }
        } else if (requested_benchmarks.size() != 0) {
            combine(actual_benchrark_list, requested_benchmarks);
        }
        diff(actual_bechmark_list, all_benchmarks, missing);
        if (missing.size() != 0) {
            // print ERROR
            // return 1
        }
#else 
        actual_benchmark_list = requested_benchmarks;            
#endif   
        OriginalBenchmarkSuite_MPI1::prepare(parser.dump());        
        //BenchmarkSuite<BS_OSU>::prepare(parser.dump());
        for (int j = 0; j < actual_benchmark_list.size(); j++) {
            smart_ptr<Benchmark> b = OriginalBenchmarkSuite_MPI1::create(actual_benchmark_list[j]);
            if (b.get() == NULL) {
                b = BenchmarkSuite<BS_OSU>::create(actual_benchmark_list[j]);
                if (b.get() == NULL) {
                    cout << "ERROR: benchmark creator failed!" << endl;
                    return 1;
                }
            }
            b->init();
            for (int n = 0; n < 40; n++)
                b->run();
            //cout << "OK" << endl;
        }
    }
    catch(exception &ex) {
        cout << "EXCEPTION: " << ex.what() << endl;
    }
    MPI_Finalize();
}
