#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <assert.h>
#include <stdexcept>
#include <iostream>

#include "benchmark.h"
#include "benchmark_suite_base.h"
#include "benchmark_suites_list.h"
#include "args_parser.h"

#include "smart_ptr.h"

using namespace std;

/*
struct BenchmarkSuiteBase {
    virtual void declare_args(args_parser &parser) = 0;
    virtual bool prepare(const args_parser &parser, const set<string> &benchs);
    template <typename T>
    virtual void get_full_list(T &all_benchmarks) const;
    template <typename T>
    virtual void get_default_list(T &benchmarks) const;
};
*/

template <benchmark_suite_t bs>
class BenchmarkSuite : public BenchmarkSuiteBase {
        static map<string, const Benchmark*> *pnames;
        static BenchmarkSuite<bs> *instance;
    public:   
        static BenchmarkSuite<bs> &get_instance() { 
            if (instance == NULL) {
                instance = new BenchmarkSuite<bs>(); 
                BenchmarkSuitesCollection::register_elem(instance);
            }
            return *instance; 
        }
        static void init() {
            set<string> benchs;
            get_full_list(benchs);
            for (set<string>::iterator it = benchs.begin(); it != benchs.end(); ++it) {
                smart_ptr<Benchmark> b = get_instance().create(*it);
                if (!b->init_description())
                    throw logic_error("BenchmarkSuite<BS_MPI1>: wrong description of one of benchmarks in suite");
            }
        }

        virtual void declare_args(args_parser &parser) const; 
        virtual bool prepare(const args_parser &parser, const set<string> &benchs); 
        static void register_elem(const Benchmark *elem) { get_instance().do_register_elem(elem); }
        static void get_full_list(std::set<std::string> &all_benchmarks) { 
            get_instance().do_get_full_list(all_benchmarks); 
        }
        virtual smart_ptr<Benchmark> create(const string &s) { return get_instance().do_create(s); }
        
    protected:
        void do_register_elem(const Benchmark *elem) {
            assert(elem != NULL);
            string name = elem->get_name();
            assert(name != "(none)");
            if (pnames == NULL) {
                pnames = new map<string, const Benchmark*>();
            }
            if (pnames->find(name) == pnames->end())
                (*pnames)[name] = elem;
        }
        smart_ptr<Benchmark> do_create(const string &s) {
            if (pnames == NULL) {
                pnames = new map<string, const Benchmark*>();
            }
            const Benchmark *elem = (*pnames)[s];
            if (elem == NULL)
                return smart_ptr<Benchmark>((Benchmark *)0);
            return smart_ptr<Benchmark>(elem->create_myself());
        }
        void do_get_full_list(std::set<std::string> &all_benchmarks) {
            std::insert_iterator<std::set<std::string> > insert(all_benchmarks, all_benchmarks.end());
            for (map<string, const Benchmark*>::iterator it = pnames->begin();
                 it != pnames->end();
                 ++it) {
                *insert++ = it->first;
            }
        }
    public:        
        virtual void get_bench_list(std::set<std::string> &benchs, BenchListFilter filter = ALL_BENCHMARKS) const;
        virtual const string get_name() const;
 
        BenchmarkSuite() { }
        ~BenchmarkSuite() { if (pnames != 0) delete pnames; }
    private:
        BenchmarkSuite &operator=(const BenchmarkSuite &) { return *this; }
        BenchmarkSuite(const BenchmarkSuite &) {}
};


#define DEFINE_INHERITED(CLASS, SUITE_CLASS) virtual const string get_name() const { return name; } \
    virtual Benchmark *create_myself() const { return new CLASS; }

//    CLASS() { SUITE_CLASS::register_elem(this); } \
//    virtual const string get_name() const { return name; } \
//    virtual Benchmark *create_myself() const { return new CLASS; } \

#define DECLARE_INHERITED(CLASS, NAME) namespace { CLASS elem_ ## NAME; } const char *CLASS::name = #NAME; 

