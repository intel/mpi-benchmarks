#include "scope.h"

using namespace std;

bool ScopeIterator::operator==(const ScopeIterator &other) { 
    return other.scope.sequence.size() == scope.sequence.size() && other.n == n; 
}

bool ScopeIterator::operator!=(const ScopeIterator &other) { 
    return !operator==(other); 
}

ScopeIterator &ScopeIterator::operator++() { 
    n++; 
    return *this; 
}

ScopeIterator ScopeIterator::operator++(int) { 
    ScopeIterator tmp(*this); 
    operator++(); 
    return tmp; 
}

pair<int, int> ScopeIterator::operator*() { 
    return scope.sequence[n]; 
}

ScopeIterator Scope::begin() { 
    assert(formed); 
    return ScopeIterator(*this, 0); 
}

ScopeIterator Scope::end() { 
    assert(formed); 
    return ScopeIterator(*this, sequence.size()); 
}


/*
struct SingleTransferScope : public Scope {
    int npmin, npmax;
    void fill_lens(vector<int> alens) { lens = alens; }
    void add_len(int len) { lens.push_back(len); }
    void fill_nps(int _npmin, int _npmax) { npmin = _npmin; npmax = _npmax; }
    void commit() {
        formed = true;
        assert(sequence.size() == 0);
        assert(lens.size() != 0);
        assert(npmax >= npmin && npmin != 0);
        for (int np = npmin; np < npmax; np += np) {
            for (size_t i = 0; i < lens.size(); i++) {
                sequence.push_back(pair<int, int>(np, lens[i]));
            }
        }
    }
};
*/
