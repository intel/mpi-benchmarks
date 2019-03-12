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

#include <vector>
#include <string>
#include <set>
#include <assert.h>
#include <iostream>
#include <algorithm>

#include "any.h"

class Scope;

struct scope_item { 
    int np; 
    size_t len; 
    any extra_fields;
    scope_item(int _len) : np(0), len(_len) { }
    scope_item(int _np, int _len) : np(_np), len(_len) { }
    scope_item(int _np, int _len, any _extra) : np(_np), len(_len), extra_fields(_extra) { }
};

struct ScopeIterator {
    ScopeIterator(Scope &_scope, int _n) : scope(_scope), n(_n) { }
    Scope &scope;
    int n;
    bool operator==(const ScopeIterator &other);
    bool operator!=(const ScopeIterator &other);
    ScopeIterator &operator++();
    ScopeIterator operator++(int);
    scope_item operator*();
};

struct Scope {
    friend class ScopeIterator;
    typedef ScopeIterator iterator;
    Scope() : formed(false) {}
    bool formed;
    std::vector<scope_item> sequence;
    ScopeIterator begin();
    ScopeIterator end();
    virtual void commit() { formed = true; }
    int get_max_len() { 
        assert(formed);  
        std::vector<int> lens;
        for (std::vector<scope_item>::iterator it = sequence.begin();
                it != sequence.end(); it++) {
            lens.push_back(it->len);
        }
        if (lens.size() == 0)
            return 0;    
        return *(std::max_element(lens.begin(), lens.end()));
    }
    virtual ~Scope() {};
};

struct VarLenScope : public Scope {
    int first_log, last_log;
    std::vector<int> lens;
    VarLenScope(int _first, int _last) : first_log(_first), last_log(_last) {
        for (int i = first_log; i <= last_log; i++) {
            lens.push_back(1 << i);
        }
        commit();
    }
    VarLenScope(const std::vector<int> &alens) : lens(alens) {
        commit();
    }
    VarLenScope(int *alens, size_t n) {
        for (size_t i = 0; i < n; i++) {
            lens.push_back(alens[i]);
        }
        commit();
    }

    virtual void commit() {
        for (size_t i = 0; i < lens.size(); i++) {
            sequence.push_back(scope_item(lens[i]));
        }
        formed = true;
    }
    virtual ~VarLenScope() {};
};

struct NPLenCombinedScope : public Scope {
    std::vector<int> lens;
    std::vector<int> nps;
    size_t nmodes;
    NPLenCombinedScope() : nmodes(1) {}
    void fill_lens(std::vector<int> _lens) { lens = _lens; }
    void add_len(int len) { lens.push_back(len); }
    void add_np(int np) { nps.push_back(np); }
    void add_nmodes(size_t _nmodes) { nmodes = _nmodes; }
    virtual void commit() {
        formed = true;
        assert(sequence.size() == 0);
        for (size_t i = 0; i < nps.size(); i++) {
            for (size_t k = 0; k < nmodes; k++) {
                for (size_t j = 0; j < lens.size(); j++) {
                    any mode = smart_ptr<int>(new int(k));
                    sequence.push_back(scope_item(nps[i], lens[j], mode));
                }
            }
        }
    }
    virtual ~NPLenCombinedScope() {};
};

