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
