#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <assert.h>
#include <stdexcept>
#include <iostream>

#include "benchmark_suites_list.h"
#include "args_parser.h"

#include "smart_ptr.h"

using namespace std;
//struct benchmark_not_found : public std::exception {
//    string name;
//    benchmark_not_found(string aname) : name(aname) {}
//    virtual ~benchmark_not_found() throw (){}
//    virtual const char* what() const throw (){
//       return name.c_str();
//    }
//};

class Benchmark {
    public:
        Benchmark() : initialized(false) {}
        virtual const string get_name() const = 0;
        virtual Benchmark* create_myself() const = 0;
        virtual bool init_description() = 0;
        virtual void init() = 0;
        virtual void run() = 0;
        virtual bool is_default() = 0;
        virtual ~Benchmark() { }
        bool initialized;
    private:
        Benchmark &operator=(const Benchmark &) { return *this; }
        Benchmark(const Benchmark &) {}
};

template <benchmark_suite_t bs>
class BenchmarkSuite {
        static map<string, const Benchmark*> *pnames;
    public:   
        static void init(); 
        static void declare_args(args_parser &parser); 
        static bool prepare(const args_parser &parser, set<string> &benchs); 
        static void register_elem(const Benchmark *elem) {
            assert(elem != NULL);
            string name = elem->get_name();
            assert(name != "(none)");
            if (pnames == NULL) {
                pnames = new map<string, const Benchmark*>();
            }
            if (pnames->find(name) == pnames->end())
                (*pnames)[name] = elem;
        }
        static smart_ptr<Benchmark> create(const string &s) {
            if (pnames == NULL) {
                pnames = new map<string, const Benchmark*>();
            }
            const Benchmark *elem = (*pnames)[s];
    //        if (elem == NULL)
    //            throw std::invalid_argument(s);
            if (elem == NULL)
                return smart_ptr<Benchmark>((Benchmark *)0);
            return smart_ptr<Benchmark>(elem->create_myself());
        }
        template <typename T>
        static void get_full_list(T &all_benchmarks) {
            std::insert_iterator<T> insert(all_benchmarks, all_benchmarks.end());
            for (map<string, const Benchmark*>::iterator it = pnames->begin();
                 it != pnames->end();
                 it++) {
                *insert++ = it->first;
            }
        }
        BenchmarkSuite() {}
        ~BenchmarkSuite() { if (pnames != 0) delete pnames; }
    private:
        BenchmarkSuite &operator=(const BenchmarkSuite &) { return *this; }
        BenchmarkSuite(const BenchmarkSuite &) {}
};


#define DEFINE_INHERITED(CLASS, SUITE_CLASS)     CLASS() { SUITE_CLASS::register_elem(this); } \
    virtual const string get_name() const { return name; } \
    virtual Benchmark *create_myself() const { return new CLASS; } \

#define DECLARE_INHERITED(CLASS, NAME) namespace { CLASS elem_ ## NAME; } const char *CLASS::name = #NAME; 

