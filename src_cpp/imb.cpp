/*****************************************************************************
 *                                                                           *
 * Copyright 2016-2019 Intel Corporation.                                    *
 *                                                                           *
 *****************************************************************************

This code is covered by the Community Source License (CPL), version
1.0 as published by IBM and reproduced in the file "license.txt" in the
"license" subdirectory. Redistribution in source and binary form, with
or without modification, is permitted ONLY within the regulations
contained in above mentioned license.

Use of the name and trademark "Intel(R) MPI Benchmarks" is allowed ONLY
within the regulations of the "License for Use of "Intel(R) MPI
Benchmarks" Name and Trademark" as reproduced in the file
"use-of-trademark-license.txt" in the "license" subdirectory.

THE PROGRAM IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT
LIMITATION, ANY WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT,
MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Each Recipient is
solely responsible for determining the appropriateness of using and
distributing the Program and assumes all risks associated with its
exercise of rights under this Agreement, including but not limited to
the risks and costs of program errors, compliance with applicable
laws, damage to or loss of data, programs or equipment, and
unavailability or interruption of operations.

EXCEPT AS EXPRESSLY SET FORTH IN THIS AGREEMENT, NEITHER RECIPIENT NOR
ANY CONTRIBUTORS SHALL HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING
WITHOUT LIMITATION LOST PROFITS), HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OR
DISTRIBUTION OF THE PROGRAM OR THE EXERCISE OF ANY RIGHTS GRANTED
HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF
YOUR JURISDICTION. It is licensee's responsibility to comply with any
export regulations applicable in licensee's jurisdiction. Under
CURRENT U.S. export regulations this software is eligible for export
from the U.S. and can be downloaded by or otherwise exported or
reexported worldwide EXCEPT to U.S. embargoed destinations which
include Cuba, Iraq, Libya, North Korea, Iran, Syria, Sudan,
Afghanistan and any other country to which the U.S. has embargoed
goods and services.

 ***************************************************************************
*/

#include <mpi.h>
#include <stdexcept>
#include <fstream>
#include <algorithm>

#include "smart_ptr.h"
#include "args_parser.h"
#include "benchmark.h"
#include "benchmark_suites_collection.h"
#include "utils.h"
#include "scope.h"
#include "benchmark_suite.h"

using namespace std;

extern void check_parser();

int main(int argc, char * *argv)
{
    bool no_mpi_init_flag = true;
    int return_value = 0;
    int rank = 0, size = 0;
    const char *program_name = "Intel(R) MPI Benchmarks 2019 Update 2";
    std::ostringstream output;

    // Some unit tests for args parser
#if 0
    check_parser();
    return 1;
#endif    

    
    try {
        // Allow very first init steps for each suite -- each benchmark
        // is allowed to init flags and do other fundamental things before MPI_Init and
        // even args parsing
        BenchmarkSuitesCollection::init_registered_suites();

        // Do basic initialisation of exapected args
        //args_parser parser(argc, argv, "/", ':');
        //args_parser parser(argc, argv, "--", '=');
        args_parser parser(argc, argv, "-", ' ', output);

        parser.set_program_name(program_name);
        parser.set_flag(args_parser::ALLOW_UNEXPECTED_ARGS);

        parser.add<string>("thread_level", "single").
               set_caption("single|funneled|serialized|multiple|nompinit").
               set_description(
                   "Sets up the type of MPI_Init call to use:\n"
                   "single: MPI_Init\n"
                   "funneled: MPI_Init_thread with MPI_THREAD_FUNNELED\n"
                   "serialized: MPI_Init_thread with MPI_THREAD_SERIALIZED\n"
                   "multiple: MPI_Init_thread with MPI_THREAD_MULTIPLE\n"
                   "nompiinit: don't call MPI_Init (the MPI_Init call may be made then in error case\n"
                   "to prevent rubbish output\n");
        parser.add<string>("input", "").set_caption("filename").
               set_description(
                   "The argument after -input is a filename is any text file containing, line by line,\n" 
                   "benchmark names facilitates running particular benchmarks as compared to\n"
                   "using the command line.\n"
                   "\n"
                   "default:\n"
                   "no input file exists\n");

        parser.add_vector<string>("include", "").set_caption("benchmark[,benchmark,[...]").
               set_description("The argument after -include is one or more benchmark names separated by comma");
        parser.add_vector<string>("exclude", "").set_caption("benchmark[,benchmark,[...]").
               set_description("The argument after -exclude is one or more benchmark names separated by comma");

        // Extra non-option arguments 
        parser.set_current_group("EXTRA_ARGS");
        parser.add_vector<string>("(benchmarks)", "").set_caption("benchmark[,benchmark,[...]]"); 

        parser.set_default_current_group();

        // Now fill in bechmark suite related args
        if (!BenchmarkSuitesCollection::declare_args(parser, output)) {
            throw runtime_error("one or more benchmark suites failed on options declaration stage");
        }

        // "system" option args to do special things, not dumped to files
        parser.set_current_group("SYS");
#ifdef WITH_YAML_CPP        
        parser.add<string>("dump", "").set_caption("config.yaml").
               set_description(
                   "Dump the YAML config file with the set of actual options for\n"
                   "the benchmark session. Parameter sets up the config file name\n");
        parser.add<string>("load", "").set_caption("config.yaml").
               set_description(
                   "Load session options from YAML config file given as a parameter\n");
#endif        
        parser.add_flag("list").
               set_description(
                   "Prints out all the benchmark names available in this IMB build.\n"
                   "The information about the benchmarks suite each benchmark belongs to\n"
                   "and the benchmark description (if available) is printed out also\n");
        parser.set_default_current_group();
         
        if (!parser.parse()) {
            throw 1;
        }
        
#ifdef WITH_YAML_CPP        
        string infile;  
        infile = parser.get<string>("load");
        if (infile != "") {
            ifstream in(infile.c_str(), ios_base::in);
            parser.load(in);
            if (!parser.parse()) {
                throw runtime_error("input config file parse error");
            }
        }
        string outfile;  
        outfile = parser.get<string>("dump");
        if (outfile != "") {
            string out;
            out = parser.dump();
            ofstream of(outfile.c_str(), ios_base::out);
            of << out;
        }
#endif
        
        vector<string> requested_benchmarks, to_include, to_exclude;
        parser.get<string>("(benchmarks)", requested_benchmarks);
        parser.get_unknown_args(requested_benchmarks);
        parser.get<string>("include", to_include);
        parser.get<string>("exclude", to_exclude);

        string filename = parser.get<string>("input");
        if (filename != "") {
            FILE *t = fopen(filename.c_str(), "r");
            if (t == NULL) {
                throw runtime_error("can't open a file given in -input option");
            }
            char input_line[72+1], name[32+1];
            while (fgets(input_line, 72, t)) {
                if (input_line[0] != '#' && strlen(input_line) > 0) {
                    sscanf(input_line, "%32s", name);
                    requested_benchmarks.push_back(name);
                }
            }
            fclose(t);
        }


        // Complete benchmark list filling in: combine -input, -include, -exclude options,
        // make sure all requested benchmarks are found
        vector<string> default_benchmarks, all_benchmarks;
        vector<string> actual_benchmark_list;
        vector<string> benchmarks_to_run;
        vector<string> missing;
        map<string, set<string> > by_suite;
        BenchmarkSuitesCollection::get_full_list(all_benchmarks, by_suite);
        BenchmarkSuitesCollection::get_default_list(default_benchmarks);
        if (parser.get<bool>("list")) {
            output << program_name << endl;
            output << "List of benchmarks:" << endl;
            for (map<string, set<string> >::iterator it_s = by_suite.begin(); 
                 it_s != by_suite.end(); ++it_s) {
                set<string> &benchmarks = it_s->second;
                string sn = it_s->first;
                if (sn == "__generic__")
                    continue;
                output << sn << ":" << endl;
                for (set<string>::iterator it_b = benchmarks.begin(); 
                     it_b != benchmarks.end(); ++it_b) {
                    smart_ptr<Benchmark> b = BenchmarkSuitesCollection::create(*it_b);

                    if (b.get() == NULL) exit(1);

                    string bn = b->get_name();
                    vector<string> comments = b->get_comments();
                    output << "    " << bn;
                    if (!b->is_default()) output << " (non-default)";
                    output << endl;
                    for (size_t i = 0; i < comments.size(); i++)
                       output << "        " << comments[i] << endl;
                }
            }
            throw 0;
        }
        {
            using namespace set_operations;
           
            preprocess_list(requested_benchmarks);
            preprocess_list(to_include);
            preprocess_list(to_exclude);
            preprocess_list(all_benchmarks);
            preprocess_list(default_benchmarks);

            if (requested_benchmarks.size() != 0) {
                combine(to_include, requested_benchmarks);
            } else {
                combine(actual_benchmark_list, default_benchmarks);
            }
            exclude(to_include, to_exclude);
            exclude(actual_benchmark_list, to_exclude);
            combine(to_include, actual_benchmark_list);
            actual_benchmark_list = to_include;
            missing = actual_benchmark_list;
            exclude(missing, all_benchmarks);
            if (missing.size() != 0) {
                exclude(actual_benchmark_list, missing);
                if (actual_benchmark_list.size() == 0) {
                    combine(actual_benchmark_list, default_benchmarks);
                }
            }

            // Change benchmark names to their canonical form
            all_benchmarks.resize(0);
            by_suite.clear();
            BenchmarkSuitesCollection::get_full_list(all_benchmarks, by_suite);
            for (size_t i = 0; i < actual_benchmark_list.size(); i++) {
                string b = to_lower(actual_benchmark_list[i]);
                for (size_t i = 0; i < all_benchmarks.size(); i++) {
                    if (to_lower(all_benchmarks[i]) == b) {
                        benchmarks_to_run.push_back(all_benchmarks[i]);
                    }
                }
            }
        }

        // Do aproppriate MPI_Init call
        string mpi_init_mode = parser.get<string>("thread_level");
        int required_mode, provided_mode;
        if (mpi_init_mode == "single") {
            no_mpi_init_flag = false;
            required_mode = MPI_THREAD_SINGLE;
        } else if (mpi_init_mode == "funneled") {
            no_mpi_init_flag = false;
            required_mode = MPI_THREAD_FUNNELED;
        } else if (mpi_init_mode == "serialized") {
            no_mpi_init_flag = false;
            required_mode = MPI_THREAD_SERIALIZED;
        } else if (mpi_init_mode == "multiple") {
            no_mpi_init_flag = false;
            required_mode = MPI_THREAD_MULTIPLE;
        } else if (mpi_init_mode == "nompiinit") {
            ;
        } else {
            throw logic_error("wrong value of `thread_level' option");
        }
        if (!no_mpi_init_flag) {
            MPI_Init_thread(&argc, (char ***)&argv, required_mode, &provided_mode);
            MPI_Comm_size(MPI_COMM_WORLD, &size);
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            if (required_mode != provided_mode) {
                throw logic_error("can't setup a required MPI threading mode");
            }
        }
 
        //---------------------------------------------------------------------
        // ACTUAL BENCHMARKING
        //
        // 1, Preparation phase on suite level
        if (!BenchmarkSuitesCollection::prepare(parser, benchmarks_to_run, missing, output)) {
            throw logic_error("One or more benchmark suites failed at preparation stage");
        }
        {
            using namespace set_operations;
            vector<string> benchmarks_to_exclude = benchmarks_to_run;
            all_benchmarks.clear();
            BenchmarkSuitesCollection::get_full_list(all_benchmarks, by_suite);
            exclude(benchmarks_to_exclude, all_benchmarks);
            exclude(benchmarks_to_run, benchmarks_to_exclude);
        }
        if (rank == 0) {
           cout << output.str();
           output.str("");
           output.clear();
        }

        // 2. All benchmarks wrappers constructors, initializers and scope definition
        typedef pair<smart_ptr<Benchmark>, smart_ptr<Scope> > item;
        typedef vector<item> running_sequence;
        running_sequence sequence;
        for (vector<string>::iterator it = benchmarks_to_run.begin(); 
             it != benchmarks_to_run.end(); ++it) {
            smart_ptr<Benchmark> b = BenchmarkSuitesCollection::create(*it);
            if (b.get() == NULL) {
                throw logic_error("benchmark creator failed!");
            }
            b->init();
            smart_ptr<Scope> scope = b->get_scope();
            sequence.push_back(item(b, scope));
        }

        // 3. Actual running cycle
        for (running_sequence::iterator it = sequence.begin(); it != sequence.end(); ++it) {
            smart_ptr<Benchmark> &b = it->first;
            smart_ptr<Scope> &scope = it->second;
            for (Scope::iterator s = scope->begin(); s != scope->end(); ++s)
                b->run(*s);
        }

        // 4. Finalize cycle
        for (running_sequence::iterator it = sequence.begin(); it != sequence.end(); ++it) {
            smart_ptr<Benchmark> &b = it->first;
            b->finalize();
        }

        // 5. Final steps on suite-level
        BenchmarkSuitesCollection::finalize(benchmarks_to_run, output);
        if (rank == 0) {
           cout << output.str();
           output.str("");
           output.clear();
        }
    }
    catch(exception &ex) {
        if (no_mpi_init_flag) {
            MPI_Init(NULL, NULL);
            no_mpi_init_flag = false;
        }
        if (!no_mpi_init_flag && rank == 0) {
            cout << "EXCEPTION: " << ex.what() << endl;
            cout << output.str();
        }
        return_value = 1;
    }
    catch(int ret) {
        if (no_mpi_init_flag) {
            MPI_Init(NULL, NULL);
            no_mpi_init_flag = false;
        }
        if (!no_mpi_init_flag && rank == 0) {
            cout << output.str();
        }
        return_value = ret;
    }
    if (!no_mpi_init_flag)
        MPI_Finalize();
    return return_value;
}
