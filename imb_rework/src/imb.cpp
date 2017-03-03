#include <mpi.h>
#include "args_parser.h"
#include "smart_ptr.h"
#include <stdexcept>
#include <fstream>
#include <algorithm>

#include "benchmark_suite.h"
#include "benchmark_suite_MPI1.h"
#include "utils.h"

#include "scope.h"


using namespace std;

extern void check_parser();



int main(int argc, char **argv)
{
#if 0
    check_parser();
#endif    

#ifdef MPI1    
    OriginalBenchmarkSuite_MPI1::init();
#endif
#ifdef OSU
    BenchmarkSuite<BS_OSU>::init();
#endif    


    MPI_Init(&argc, &argv);
    try {
        //args_parser parser(argc, argv, "/", ':');
        //args_parser parser(argc, argv, "--", '=');
        args_parser parser(argc, argv, "-", ' ');

        parser.add_option_with_defaults<string>("input", "").
            set_caption("filename");
        parser.add_option_with_defaults_vec<string>("include").
            set_caption("benchmark[,benchmark,[...]");
        parser.add_option_with_defaults_vec<string>("exclude").
            set_caption("benchmark[,benchmark,[...]");

        // extra non-option arguments 
        parser.set_current_group("EXTRA_ARGS");
        parser.add_option_with_defaults_vec<string>("(benchmarks)").
            set_caption("benchmark[,benchmark,[...]]"); 
        parser.set_default_current_group();

        // bechmark suite related args
        BenchmarkSuitesCollection::declare_args(parser);

        // "system" option args to do special things, not dumped to files
        parser.set_current_group("SYS");
        parser.add_option_with_defaults<string>("dump", "").
            set_caption("config.yaml");
        parser.add_option_with_defaults<string>("load", "").
            set_caption("config.yaml");
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

        string filename = parser.get_result<string>("input");
        if (filename != "") {
            int n_cases = 0;
            FILE *t = fopen(filename.c_str(), "r");
            if (t == NULL) {
                cout << "ERROR: can't open a file given in -input option" << endl;
                return 1;
            }
            char inp_line[72], nam[32];
            while (fgets(inp_line, 72, t)) {
                if (inp_line[0] != '#' && strlen(inp_line) - 1) {
                    sscanf(inp_line, "%32s", nam);
                    n_cases++;
                    requested_benchmarks.push_back(nam);
                }
            }
            fclose(t);
        }


        set<string> default_benchmarks, all_benchmarks;
        set<string> actual_benchmark_list;
        BenchmarkSuitesCollection::get_full_list(all_benchmarks);
        BenchmarkSuitesCollection::get_default_list(default_benchmarks);
        {
            using namespace set_operations;
            
            preprocess_list(requested_benchmarks);
            preprocess_list(to_include);
            preprocess_list(to_exclude);

            preprocess_list(all_benchmarks);
            preprocess_list(default_benchmarks);

            if (to_include.size() != 0 || to_exclude.size() != 0) {
                if (requested_benchmarks.size() != 0) {
                    // FIXME we can actually work it out
                    cout << "ERROR: can't combine -include and -exclude options with explicit benchmark list" << endl;
                    return 1;
                } else {
                    combine(actual_benchmark_list, default_benchmarks);
                    combine(actual_benchmark_list, to_include);
                    exclude(actual_benchmark_list, to_exclude);
               }
            } else { 
                if (requested_benchmarks.size() != 0) {
                    combine(actual_benchmark_list, requested_benchmarks);
                } else {
                    combine(actual_benchmark_list, default_benchmarks);
                }
            }
            set<string> missing;
            diff(actual_benchmark_list, all_benchmarks, missing);
            if (missing.size() != 0) {
                cout << "Benchmarks not found:" << endl;
                for (set<string>::iterator it = missing.begin(); it != missing.end(); ++it) {
                    cout << ">> " << *it << endl;
                }
                return 1;
            }
        }

        if (!BenchmarkSuitesCollection::prepare(parser, actual_benchmark_list)) {
            cout << "One or more benchmark suites failed at preparation stage" << endl;
            return 1;
        }        
        for (set<string>::iterator it = actual_benchmark_list.begin(); it != actual_benchmark_list.end(); ++it) {
            smart_ptr<Benchmark> b = BenchmarkSuitesCollection::create(*it);
            if (b.get() == NULL) {
                cout << "ERROR: benchmark creator failed!" << endl;
                return 1;
            }
            b->init();
            smart_ptr<Scope> scope = b->get_scope();            
            for (Scope::iterator it = scope->begin(); it != scope->end(); ++it)
                b->run(*it);
        }
    }
    catch(exception &ex) {
        cout << "EXCEPTION: " << ex.what() << endl;
    }
    MPI_Finalize();
}
