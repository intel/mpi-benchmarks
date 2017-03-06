#pragma once

#include <vector>
#include <string>
#include <set>
#include <assert.h>
#include <iostream>

class Scope;

struct ScopeIterator {
    //ScopeIterator() : n(0) {}
    ScopeIterator(Scope &_scope, int _n) : scope(_scope), n(_n) { }

    //vector<pair<int, int> > sequence;
    Scope &scope;
    int n;
    bool operator==(const ScopeIterator &other);
    bool operator!=(const ScopeIterator &other);
    ScopeIterator &operator++();
    ScopeIterator operator++(int);
    std::pair<int, int> operator*();
};

struct Scope {
    friend class ScopeIterator;
    typedef ScopeIterator iterator;
    Scope() : formed(false) {}
    bool formed;
    std::vector<std::pair<int, int> > sequence;
    ScopeIterator begin();
    ScopeIterator end();
    virtual void commit() { formed = true; }
};

struct VarLenLogScope : public Scope {
    int first, last;
    VarLenLogScope(int _first, int _last) : first(_first), last(_last) {
        commit();
    }
    virtual void commit() {
        for (int i = first; i <= last; i++) {
            sequence.push_back(std::pair<int, int>(0, 1 << i));
        }
        formed = true;
    }
};

struct NPLenCombinedScope : public Scope {
    std::vector<int> lens;
    std::vector<int> nps;
    void fill_lens(std::vector<int> _lens) { lens = _lens; }
    void add_len(int len) { lens.push_back(len); }
    void add_np(int np) { nps.push_back(np); }
    virtual void commit() {
        formed = true;
        assert(sequence.size() == 0);
        assert(lens.size() != 0);
        assert(nps.size() != 0);
        for (size_t i = 0; i < nps.size(); i++) {
            for (size_t j = 0; j < lens.size(); j++) {
                sequence.push_back(std::pair<int, int>(nps[i], lens[j]));
            }
        }
    }
};

