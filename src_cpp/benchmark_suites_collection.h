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
