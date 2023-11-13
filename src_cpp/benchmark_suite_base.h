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
#include "benchmark.h"

class args_parser;
class Benchmark;

struct BenchmarkSuiteBase {
    enum BenchListFilter { ALL_BENCHMARKS, DEFAULT_BENCHMARKS };
    virtual void init() {}
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
    virtual void finalize(const std::vector<std::string> &, std::ostream &output = std::cout) {
        UNUSED(output);
    }
    virtual void get_bench_list(std::set<std::string> &, BenchListFilter filter = ALL_BENCHMARKS) const { UNUSED(filter); }
    virtual void get_bench_list(std::vector<std::string> &, BenchListFilter filter = ALL_BENCHMARKS) const { UNUSED(filter); }
    virtual smart_ptr<Benchmark> create(const std::string &) { return smart_ptr<Benchmark>(); }
    virtual const std::string get_name() const = 0;
    virtual any get_parameter(const std::string &key) { UNUSED(key); return any(); }
};

