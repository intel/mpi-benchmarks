#pragma once

#include <vector>
#include <string>
#include <set>
#include <assert.h>
#include <iostream>
#include <algorithm>

class Scope;

struct ScopeIterator {
    ScopeIterator(Scope &_scope, int _n) : scope(_scope), n(_n) { }
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
        for (int i = 0; i < n; i++) {
            lens.push_back(alens[i]);
        }
        commit();
    }

    virtual void commit() {
        for (int i = 0; i < lens.size(); i++) {
            sequence.push_back(std::pair<int, int>(0, lens[i]));
        }
        formed = true;
    }
    int get_max_len() {
        if (lens.size() == 0)
            return 0;
        return *(std::max_element(lens.begin(), lens.end()));
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

