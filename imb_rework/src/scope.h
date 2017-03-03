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

struct SingleTransferScope : public Scope {
    int npmin, npmax;
    std::vector<int> lens;
    void fill_lens(std::vector<int> _lens) { lens = _lens; }
    void add_len(int len) { lens.push_back(len); }
    void fill_nps(int _npmin, int _npmax) { npmin = _npmin; npmax = _npmax; }
    virtual void commit() {
        formed = true;
        assert(sequence.size() == 0);
        assert(lens.size() != 0);
        assert(npmax >= npmin && npmin != 0);
        for (int np = npmin; np <= npmax; np++) {
            for (size_t i = 0; i < lens.size(); i++) {
                sequence.push_back(std::pair<int, int>(np, lens[i]));
            }
        }
    }
};

