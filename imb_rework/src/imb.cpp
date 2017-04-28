/*****************************************************************************
 *                                                                           *
 * Copyright (c) 2016-2017 Intel Corporation.                                *
 * All rights reserved.                                                      *
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
#include "args_parser.h"
#include "smart_ptr.h"
#include <stdexcept>
#include <fstream>
#include <algorithm>

#include "benchmark_suite.h"
#include "utils.h"
#include "scope.h"

#ifdef MPI1
#include "legacy_MPI1_suite.h"
#endif
#ifdef MT
#include "MT_suite.h"
#endif
#ifdef HALO
#include "halo_suite.h"
#endif

using namespace std;

extern void check_parser();

int main(int argc, char **argv)
{
    bool no_mpi_init_flag = true;
    int rank = 0, size = 0;
#if 0
    check_parser();
#endif    

#ifdef MPI1    
    OriginalBenchmarkSuite_MPI1::init();
#endif
#ifdef OSU
    BenchmarkSuite<BS_OSU>::init();
#endif    
#ifdef MT
    MTBenchmarkSuite::init();
#endif    
#ifdef HALO
    HALOBenchmarkSuite::init();
#endif    

#ifdef EXAMPLE
    BenchmarkSuite<BS_EXAMPLE>::init();
#endif    



    try {
        //args_parser parser(argc, argv, "/", ':');
        //args_parser parser(argc, argv, "--", '=');
        args_parser parser(argc, argv, "-", ' ');

        parser.set_flag(args_parser::ALLOW_UNEXPECTED_ARGS);

        parser.add_option_with_defaults<string>("thread_level", "single").
            set_caption("single|funneled|serialized|multiple|nompinit");
        parser.add_option_with_defaults<string>("input", "").
            set_caption("filename");
        parser.add_option_with_defaults_vec<string>("include").
            set_caption("benchmark[,benchmark,[...]");
        parser.add_option_with_defaults_vec<string>("exclude").
            set_caption("benchmark[,benchmark,[...] ...");

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
        parser.get_unknown_args(requested_benchmarks);
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

            if (requested_benchmarks.size() != 0) {
                combine(to_include, requested_benchmarks);
            } else {
                combine(actual_benchmark_list, default_benchmarks);
            }
            exclude(actual_benchmark_list, to_exclude);
            combine(actual_benchmark_list, to_include);

            set<string> missing;
            diff(actual_benchmark_list, all_benchmarks, missing);
            if (missing.size() != 0) {
                cout << "Benchmarks not found:" << endl;
                for (set<string>::iterator it = missing.begin(); it != missing.end(); ++it) {
                    cout << *it << endl;
                }
                return 1;
            }
        }
        string mpi_init_mode = parser.get_result<string>("thread_level");
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
            cout << "ERROR: wrong value of `thread_level' option" << endl;
            return 1;
        }
        if (!no_mpi_init_flag) {
            MPI_Init_thread(&argc, &argv, required_mode, &provided_mode);
            MPI_Comm_size(MPI_COMM_WORLD, &size);
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            if (required_mode != provided_mode) {
                throw logic_error("can't setup a required MPI threading mode");
            }
        }
 
        // 1, Preparation phase on suite level
        if (!BenchmarkSuitesCollection::prepare(parser, actual_benchmark_list)) {
            throw logic_error("One or more benchmark suites failed at preparation stage");
        }        

        // 2. All benchmarks wrappers constructors, initializers and scope definition
        typedef pair<smart_ptr<Benchmark>, smart_ptr<Scope> > item;
        typedef vector<item> running_sequence;
        running_sequence sequence;
        for (set<string>::iterator it = actual_benchmark_list.begin(); it != actual_benchmark_list.end(); ++it) {
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
        BenchmarkSuitesCollection::finalize(actual_benchmark_list);
    }
    catch(exception &ex) {
        if (!no_mpi_init_flag && rank == 0)
            cout << "EXCEPTION: " << ex.what() << endl;
    }
    if (!no_mpi_init_flag)
        MPI_Finalize();
}
