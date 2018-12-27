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
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>

#include "benchmark.h"
#include "benchmark_suites_collection.h"
#include "scope.h"
#include "utils.h"

using namespace std;


// EXAMPLE 5: generic code, Benchmark class directly inherits from the base Benchmarks class.
// Suite code directly inherits from BenchmarkSuiteBase class.
// Registration is done by rather ugly direct register_elem() call from a constructor
class ExampleBenchmark_5 : public Benchmark 
{
    virtual const string get_name() const { return "example5"; }
    virtual void run(const scope_item &) { std::cout << "Hello from example 5"; }
};

class BenchmarkSuiteExample5 : public BenchmarkSuiteBase
{
    static BenchmarkSuiteExample5 *instance;
    public:
    BenchmarkSuiteExample5() { 
        if (instance == NULL) {
            instance = (BenchmarkSuiteExample5 *)0xffff;
            instance = new BenchmarkSuiteExample5(); 
            BenchmarkSuitesCollection::register_elem(instance);
        }
    }
    virtual ~BenchmarkSuiteExample5() {
        if (instance != NULL) {
            BenchmarkSuiteExample5 *to_delete;
            to_delete = instance;
            instance = NULL;
            delete to_delete;
        }
    }
    virtual void get_bench_list(std::set<std::string> &result, 
        BenchListFilter filter = ALL_BENCHMARKS) const 
    { 
        UNUSED(filter);
        result.insert("example_bench5");
    }
    virtual void get_bench_list(std::vector<std::string> &result, 
        BenchListFilter filter = ALL_BENCHMARKS) const 
    { 
        UNUSED(filter);
        result.push_back("example_bench5");
    }
    virtual smart_ptr<Benchmark> create(const std::string &name) 
    {  
        if (name == "example_bench5") {
            return smart_ptr<Benchmark>(new ExampleBenchmark_5);
        }
        return smart_ptr<Benchmark>(NULL);
    }
    virtual const std::string get_name() const { return "example_suite5"; };
};

BenchmarkSuiteExample5 *BenchmarkSuiteExample5::instance;
namespace { BenchmarkSuiteExample5 bench_suite_ex5; }

