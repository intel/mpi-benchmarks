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
        Benchmark() {}
        virtual const string get_name() const = 0;
        virtual Benchmark* create_myself() const = 0;
        virtual void run() = 0;
        virtual ~Benchmark() { }
        Benchmark &operator=(const Benchmark &) = delete;
        Benchmark &operator=(const Benchmark &&) = delete;
        Benchmark(const Benchmark &) = delete;
        Benchmark(const Benchmark &&) = delete;
};

template <benchmark_suite_t bs>
class BenchmarkSuite {
    static map<string, const Benchmark*> *pnames;
    public:   
    static void declare_args(args_parser &parser); 
    static bool prepare(const std::string &yaml_config); 
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
    static shared_ptr<Benchmark> create(const string &s) {
        if (pnames == NULL) {
            pnames = new map<string, const Benchmark*>();
        }
        const Benchmark *elem = (*pnames)[s];
//        if (elem == NULL)
//            throw std::invalid_argument(s);
        if (elem == NULL)
            return shared_ptr<Benchmark>((Benchmark *)0);
        return shared_ptr<Benchmark>(elem->create_myself());
    }
    BenchmarkSuite() {}
    ~BenchmarkSuite() { if (pnames != 0) delete pnames; }
    BenchmarkSuite &operator=(const BenchmarkSuite &) = delete;
    BenchmarkSuite &operator=(const BenchmarkSuite &&) = delete;    
    BenchmarkSuite(const BenchmarkSuite &) = delete;
    BenchmarkSuite(const BenchmarkSuite &&) = delete;    
};


#define DEFINE_INHERITED(CLASS, SUITE)     CLASS() { BenchmarkSuite<SUITE>::register_elem(this); } \
    virtual const string get_name() const { return name; } \
    virtual Benchmark *create_myself() const { return new CLASS; } \

#define DECLARE_INHERITED(CLASS, NAME) namespace { CLASS elem_ ## NAME; } const char *CLASS::name = #NAME; 

