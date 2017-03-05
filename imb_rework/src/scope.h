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

struct NPLenCombinedScope : public Scope {
    std::vector<int> lens;
    std::vector<int> nps;
    void fill_lens(std::vector<int> _lens) { lens = _lens; }
    void add_len(int len) { lens.push_back(len); }
    void add_np(int np) { nps.push_back(np); }
//    void fill_nps(int _npmin, int _npmax) { npmin = _npmin; npmax = _npmax; }
    virtual void commit() {
        formed = true;
        assert(sequence.size() == 0);
        assert(lens.size() != 0);
        assert(nps.size() != 0);
/*
        bool do_it = true;
        while (do_it) {
            ci_np = C_INFO.w_num_procs;
            if( Bmark->RUN_MODES[0].type == ParallelTransferMsgRate ) {
                ci_np -= ci_np % 2;
                NP_min += NP_min % 2;
            }
//            if( Bmark->RUN_MODES[0].type != BTYPE_INVALID ) {
            NP=max(1,min(ci_np,NP_min));

            if( Bmark->RUN_MODES[0].type == SingleTransfer ) {
                NP = (min(2,ci_np));
            }
            // CALCULATE THE NUMBER OF PROCESSES FOR NEXT STEP 
            if( NP >= ci_np  ) {do_it=0;}
            else
            {
                NP=min(NP+NP,ci_np);
            }
            scope.add_np(NP);
        }
*/
        for (size_t i = 0; i < nps.size(); i++) {
            for (size_t j = 0; j < lens.size(); j++) {
                sequence.push_back(std::pair<int, int>(nps[i], lens[j]));
            }
        }
    }
};

