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
#include "scope.h"
#include <iostream>

#define UNUSED(expr) do { (void)(expr); } while (0)

class BenchmarkSuiteBase;
class Benchmark {
    public:
        Benchmark() : initialized(false) {}
        virtual const std::string get_name() const { return std::string(""); }
        virtual Benchmark* create_myself() const { return NULL; }
        virtual void allocate_internals() {}
        virtual bool init_description() { return true; }
        virtual void init() { } 
        virtual void run(const scope_item &) = 0;
        virtual void finalize() { }
        virtual bool is_default() { return true; }
        virtual std::vector<std::string> get_comments() { return std::vector<std::string>(0); }
        smart_ptr<Scope> get_scope() { if (scope.get() == NULL) { scope.assign(new Scope); scope->commit(); } return scope; }
        virtual ~Benchmark() { }
        bool initialized;
        BenchmarkSuiteBase *suite;
    protected:
        smart_ptr<Scope> scope;
    private:
        Benchmark &operator=(const Benchmark &) { return *this; }
        Benchmark(const Benchmark &) {}
};

#define DEFINE_INHERITED(CLASS, SUITE_CLASS) static const char *name; \
    virtual const std::string get_name() const { return name; } \
    virtual Benchmark *create_myself() const { return new CLASS; } \
    CLASS() { Benchmark::suite = SUITE_CLASS::register_elem(this); this->allocate_internals(); }

#define DECLARE_INHERITED(CLASS, NAME) namespace { CLASS elem_ ## NAME; } const char *CLASS::name = #NAME;

#ifdef __GNUC__ 
#define DECLARE_INHERITED_TEMPLATE(CLASS, NAME) namespace { CLASS elem_ ## NAME; } template<> const char *CLASS::name = #NAME;
#else
#define DECLARE_INHERITED_TEMPLATE(CLASS, NAME) namespace { CLASS elem_ ## NAME; } const char *CLASS::name = #NAME;
#endif
