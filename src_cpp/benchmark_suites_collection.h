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

 ***************************************************************************/

#pragma once

#include "benchmark_suite_base.h"
#include "utils.h"

//!!! to remove
#include <iostream>

enum benchmark_suite_t {
    BS_MPI1,
    BS_NBC,
    BS_MT,
    BS_RMA,
    BS_EXT,
    BS_IO,
    BS_GENERIC
};

class BenchmarkSuitesCollection {
    static std::map<const std::string, BenchmarkSuiteBase*> *pnames;
    public:
    static void register_elem(BenchmarkSuiteBase *elem) {
        assert(elem != NULL);
        const std::string name = elem->get_name();
        if (pnames == NULL) {
            pnames = new std::map<const std::string, BenchmarkSuiteBase*>();
        }
        if (pnames->find(name) == pnames->end()) {
            (*pnames)[name] = elem;
        }
    }
    static void get_full_list(std::vector<std::string> &all_benchmarks, 
                              std::map<std::string, std::set<std::string> > &by_suite) {
        assert(pnames != NULL);
        for (std::map<const std::string, BenchmarkSuiteBase*>::iterator it = pnames->begin();
             it != pnames->end(); ++it) {
            std::set<std::string> &benchmarks = by_suite[it->second->get_name()];
            it->second->get_bench_list(benchmarks, BenchmarkSuiteBase::ALL_BENCHMARKS);
            set_operations::combine(all_benchmarks, benchmarks);
        }
    }
    static void get_default_list(std::vector<std::string> &default_benchmarks) {
        assert(pnames != NULL);
        for (std::map<const std::string, BenchmarkSuiteBase*>::iterator it = pnames->begin();
             it != pnames->end(); ++it) {
            it->second->get_bench_list(default_benchmarks, BenchmarkSuiteBase::DEFAULT_BENCHMARKS);
        }
    }
    static void init_registered_suites() {
       assert(pnames != NULL);
        for (std::map<const std::string, BenchmarkSuiteBase*>::iterator it = pnames->begin();
             it != pnames->end(); ++it) {
                it->second->init();
        }
    }
    static bool declare_args(args_parser &parser, std::ostream &output) {
        assert(pnames != NULL);
        for (std::map<const std::string, BenchmarkSuiteBase*>::iterator it = pnames->begin();
             it != pnames->end(); ++it) {
                if (!it->second->declare_args(parser, output))
                    return false;
        }
        return true;
    }
    static bool prepare(args_parser &parser, const std::vector<std::string> &benchs,
                        const std::vector<std::string> &unknown_args, std::ostream &output) {
        assert(pnames != NULL);
        std::vector<std::string> suites_to_remove;
        for (std::map<const std::string, BenchmarkSuiteBase*>::iterator it = pnames->begin();
             it != pnames->end(); ++it) {
                if (!it->second->prepare(parser, benchs, unknown_args, output)) {
                    suites_to_remove.push_back(it->first);
                }
        }
        for (std::vector<std::string>::iterator it = suites_to_remove.begin();
                it != suites_to_remove.end(); ++it) {
            pnames->erase(*it);
        }
        return true;
    }
    static smart_ptr<Benchmark> create(const std::string &name) {
        assert(pnames != NULL);
        smart_ptr<Benchmark> b;
        for (std::map<const std::string, BenchmarkSuiteBase*>::iterator it = pnames->begin();
             it != pnames->end(); ++it) {
            b = it->second->create(name);
            if (b.get() != NULL)
                break;
        }
        return b;
    }
    static void finalize(const std::vector<std::string> &benchs, std::ostream &output) {
        assert(pnames != NULL);
        for (std::map<const std::string, BenchmarkSuiteBase*>::iterator it = pnames->begin();
             it != pnames->end(); ++it) 
                it->second->finalize(benchs, output);
    }    
};
