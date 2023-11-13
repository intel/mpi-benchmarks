/****************************************************************************
*                                                                           *
* Copyright (C) 2023 Intel Corporation                                      *
*                                                                           *
*****************************************************************************

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ****************************************************************************/


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

