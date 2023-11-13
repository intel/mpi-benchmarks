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

