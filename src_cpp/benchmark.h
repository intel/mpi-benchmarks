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
*/

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
