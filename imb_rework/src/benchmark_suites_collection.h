#pragma once

#include "benchmark_suite_base.h"

//!!!
#include <iostream>

enum benchmark_suite_t { 
#ifdef MPI1    
    BS_MPI1, 
#endif
#ifdef NBC    
    BS_NBC,
#endif
#ifdef OSU
    BS_OSU,
#endif
    BS_EXAMPLE, 
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
        if (pnames->find(name) == pnames->end()) 
            (*pnames)[name] = elem;
    }
    static void get_full_list(std::set<std::string> &all_benchmarks) {
        assert(pnames != NULL);
        for (std::map<const std::string, BenchmarkSuiteBase*>::iterator it = pnames->begin();
             it != pnames->end(); ++it) {
            it->second->get_bench_list(all_benchmarks, BenchmarkSuiteBase::ALL_BENCHMARKS);
        }
    }
    static void get_default_list(std::set<std::string> &default_benchmarks) {
        assert(pnames != NULL);
        for (std::map<const std::string, BenchmarkSuiteBase*>::iterator it = pnames->begin();
             it != pnames->end(); ++it) {
            it->second->get_bench_list(default_benchmarks, BenchmarkSuiteBase::DEFAULT_BENCHMARKS);
        }
    }
    static void declare_args(args_parser &parser) {
        assert(pnames != NULL);
        for (std::map<const std::string, BenchmarkSuiteBase*>::iterator it = pnames->begin();
             it != pnames->end(); ++it) {
                it->second->declare_args(parser);
        }
    }
    static bool prepare(args_parser &parser, const std::set<std::string> &benchs) {
        assert(pnames != NULL);
        for (std::map<const std::string, BenchmarkSuiteBase*>::iterator it = pnames->begin();
             it != pnames->end(); ++it) {
                if (!it->second->prepare(parser, benchs)) {
                    return false;
                }
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
    static void finalize(const std::set<std::string> &benchs) {
        assert(pnames != NULL);
        for (std::map<const std::string, BenchmarkSuiteBase*>::iterator it = pnames->begin();
             it != pnames->end(); ++it) 
                it->second->finalize(benchs); 
    }    
};
