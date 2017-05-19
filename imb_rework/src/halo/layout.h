#ifndef _LAYOUT_H
#define _LAYOUT_H
#include <vector>
#include <string>

class Layout {
private:
    int m_nranks;
    int m_ndims;
//    int *m_ranksperdim;
    std::vector<int> m_ranksperdim;
    //int *m_sizes;
//    int *m_mults;
    std::vector<int> m_mults;    
//    int *m_enabled;
//    std::vector<int> m
//    int m_niters;
////    int m_ppn;
    void prvals(const int *arr);
    void prvals(const std::vector<int> &arr);
    int *tokens2arr(const char *option, const std::vector<std::string> &tokens);
    void copy_scalar(Layout &to, const Layout &from);
    void copy_vector(Layout &to, const Layout &from);
    void alloc();
    void free();
public:
    Layout(int argc, char **argv);
    Layout();
    Layout(const Layout &other);
    Layout &operator=(const Layout &other);
    ~Layout();
    // linearize
    int
    substorank(const unsigned int *subs)
    {
        int rank = 0;
        // last subscript varies fastest
        for (int i = 0; i < m_ndims; ++i)
            rank += m_mults[i] * subs[i];
        return rank;
    }
    // delinearize
    void ranktosubs(int rank, unsigned int *subs)
    {
        int rem = rank;
        for (int i = 0; i < m_ndims; ++i) {
            int sub = rem / m_mults[i];
            rem %= m_mults[i];
            subs[i] = sub;
        }
    }
    void prlayout();
    void prsubs(const char *name, const unsigned *subs);
    int ndims()         { return m_ndims; }
    int nranks()        { return m_nranks; }
//    int size(int i)     { return m_sizes[i]; }
    int ranksperdim(int i) { return m_ranksperdim[i]; }
//    int enabled(int i)  { return m_enabled[i]; }
//    int niters()        { return m_niters; }
////    int ppn()           { return m_ppn; }
////    bool samenode(int n1, int n2) { return n1 / m_ppn == n2 / m_ppn; }
};
#endif
