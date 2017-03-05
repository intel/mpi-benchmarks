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


