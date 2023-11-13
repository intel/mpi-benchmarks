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

#include "smart_ptr.h"
#include "any.h"

template <benchmark_suite_t bs>
class BenchmarkSuite : public BenchmarkSuiteBase {
    public:
        typedef std::map<std::string, const Benchmark*, set_operations::case_insens_cmp> pnames_t;
    protected:
        static pnames_t *pnames;
        static BenchmarkSuite<bs> *instance;
        std::vector<std::string> names_list;
    public:   
        static BenchmarkSuite<bs> &get_instance() { 
            if (instance == NULL) {
                instance = new BenchmarkSuite<bs>(); 
                BenchmarkSuitesCollection::register_elem(instance);
            }
            return *instance; 
        }
        virtual void init() {
            std::set<std::string> benchs;
            get_full_list(benchs);
            for (std::set<std::string>::iterator it = benchs.begin(); it != benchs.end(); ++it) {
                smart_ptr<Benchmark> b = get_instance().create(*it);
                if (!b->init_description())
                    throw std::logic_error("BenchmarkSuite: wrong description of one of benchmarks in suite");
            }
        }

        virtual bool declare_args(args_parser &,
                                  std::ostream &output = std::cout) const {
            UNUSED(output);
            return true;
        } 
        virtual bool prepare(const args_parser &, const std::vector<std::string> &,
                             const std::vector<std::string> &, std::ostream &output = std::cout) {
            UNUSED(output);
            return true;
        }
        virtual void finalize(const std::vector<std::string> &,
                              std::ostream &output = std::cout) { UNUSED(output); }
        static BenchmarkSuite<bs> *register_elem(const Benchmark *elem) { get_instance().do_register_elem(elem); return instance; }
        static void get_full_list(std::set<std::string> &all_benchmarks) {
            get_instance().do_get_full_list(all_benchmarks);
        }
        static void get_full_list(std::vector<std::string> &all_benchmarks) {
            get_instance().do_get_full_list(all_benchmarks);
        }
        virtual smart_ptr<Benchmark> create(const std::string &s) { return get_instance().do_create(s); }
        virtual any get_parameter(const std::string &key) { UNUSED(key); return any(); }
        
    protected:
        void do_register_elem(const Benchmark *elem) {
            assert(elem != NULL);
            std::string name = elem->get_name();
            assert(name != "(none)");
            if (pnames == NULL) {
                pnames = new pnames_t();
            }
            if (pnames->find(name) == pnames->end()) {
                (*pnames)[name] = elem;
                names_list.push_back(name);
            }
        }
        smart_ptr<Benchmark> do_create(const std::string &s) {
            if (pnames == NULL) {
                pnames = new pnames_t();
            }
            const Benchmark *elem = (*pnames)[s];
            if (elem == NULL)
                return smart_ptr<Benchmark>((Benchmark *)0);
            return smart_ptr<Benchmark>(elem->create_myself());
        }
        template <typename T>
        void do_get_full_list(T &all_benchmarks) {
            if (pnames == NULL) {
                pnames = new pnames_t();
            }
            std::insert_iterator<T> insert(all_benchmarks, all_benchmarks.end());
            for (size_t i = 0; i < names_list.size(); i++) {
                *insert++ = names_list[i];
            }
        }
    public:
        virtual void get_bench_list(std::set<std::string> &benchs, BenchListFilter filter = ALL_BENCHMARKS) const {
            UNUSED(filter);
            get_full_list(benchs); 
        }
        virtual void get_bench_list(std::vector<std::string> &benchs, BenchListFilter filter = ALL_BENCHMARKS) const {
            UNUSED(filter);
            get_full_list(benchs); 
        }
        virtual const std::string get_name() const;
 
        BenchmarkSuite() { }
        ~BenchmarkSuite() { if (pnames != 0) delete pnames; }
    private:
        BenchmarkSuite &operator=(const BenchmarkSuite &) { return *this; }
        BenchmarkSuite(const BenchmarkSuite &) {}
};

#define DECLARE_BENCHMARK_SUITE_STUFF(SUITE, NAME) \
template<> BenchmarkSuite<SUITE>::pnames_t *BenchmarkSuite<SUITE>::pnames = 0; \
template<> BenchmarkSuite<SUITE> *BenchmarkSuite<SUITE>::instance = 0; \
template <> const std::string BenchmarkSuite<SUITE>::get_name() const { return #NAME; }
