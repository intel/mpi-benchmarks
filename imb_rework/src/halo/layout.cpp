#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "layout.h"

using namespace std;

static void
usage(int argc, char **argv)
{
    argc = 0;
    fprintf(stderr,
"Usage: %s -m N[.N ...] [-s S[.S ..]] [-e 0|1[.0|1 ...]] [-p ppn] [-c #iters]\n",
        argv[0]);
    exit(1);
}

static void
tokenize(const char *option, int argc, char **argv, int &arg, vector<string> &tokens)
{
    if (arg+1 >= argc) {
        fprintf(stderr, "No string after option %s\n", option);
        usage(argc, argv);
    }
    tokens.clear();
    ++arg;
    for (char *p = strtok(argv[arg], "."); p != NULL; p = strtok(NULL, "."))
        tokens.push_back(p);
}

int *
Layout::tokens2arr(const char *option, const vector<string> &tokens)
{
    int l = tokens.size();
    if (l != 1 && l != m_ndims) {
        fprintf(stderr, "string after %s must have 1 or %d entries\n",
            option, m_ndims);
        exit(1);
    }
    int *p = new int[m_ndims];
    if (l == 1)
        for (int i = 0; i < m_ndims; ++i)
            p[i] = atoi(tokens[0].c_str());
    else
        for (int i = 0; i < m_ndims; ++i)
            p[i] = atoi(tokens[i].c_str());
    return p;
}

void
Layout::prvals(const std::vector<int> &arr)
{
    for (int i = 0; i < m_ndims-1; ++i)
        printf("%d.", arr[i]);
    printf("%d", arr[m_ndims-1]);
}

void
Layout::prvals(const int *arr)
{
    for (int i = 0; i < m_ndims-1; ++i)
        printf("%d.", arr[i]);
    printf("%d", arr[m_ndims-1]);
}

void Layout::copy_scalar(Layout &to, const Layout &from) {
    m_nranks = from.m_nranks;
    m_ndims = from.m_ndims;
//    m_niters = from.m_niters; 
//    m_ppn = from.m_ppn;
}

void Layout::copy_vector(Layout &to, const Layout &from) {
//    //memcpy(m_ranksperdim, from.m_ranksperdim, sizeof(int) * m_ndims);
//    memcpy(m_sizes, from.m_sizes, sizeof(int) * m_ndims);
//    memcpy(m_mults, from.m_mults, sizeof(int) * m_ndims);
//    memcpy(m_enabled, from.m_enabled, sizeof(int) * m_ndims);
    m_ranksperdim = from.m_ranksperdim;
    m_mults = from.m_mults;    
}

void Layout::alloc() {
//    m_ranksperdim = (int *)malloc(sizeof(int) * m_ndims);
//    m_sizes = (int *)malloc(sizeof(int) * m_ndims);
//    m_mults = (int *)malloc(sizeof(int) * m_ndims);
//    m_enabled = (int *)malloc(sizeof(int) * m_ndims);
}

void Layout::free() {
//    ::free(m_ranksperdim); m_ranksperdim = NULL;
//    ::free(m_sizes); m_sizes = NULL;
//    ::free(m_mults); m_mults = NULL;
//    ::free(m_enabled); m_enabled = NULL;
}

Layout::Layout(const Layout &other) {
    copy_scalar(*this, other);
    alloc();
    copy_vector(*this, other);
}

Layout &Layout::operator=(const Layout &other) {
    free();
    copy_scalar(*this, other);
    alloc();
    copy_vector(*this, other);
    return *this; 
}

Layout::Layout() : m_nranks(0), m_ndims(0) 
{
    m_ndims = 2;
    alloc();
//    m_ranksperdim =  new int[m_ndims];
    m_ranksperdim.resize(m_ndims);    
    for (int i = 0; i < m_ndims; i++)
        m_ranksperdim[i] = 1;
//    if (m_ndims == 0)
//        usage(argc, argv);
//    if (m_sizes == 0) {
//        m_sizes = new int[m_ndims];
//        for (int i = 0; i < m_ndims; ++i)
//            m_sizes[i] = 1024*1024;
//    }
//    if (m_enabled == 0) {
//        m_enabled = new int[m_ndims];
//        for (int i = 0; i < m_ndims; ++i)
//            m_enabled[i] = 1;
//    }
    m_nranks = 1;
    for (int i = 0; i < m_ndims; ++i)
        m_nranks *= m_ranksperdim[i];
//    m_mults = new int[m_ndims];
    m_mults.resize(m_ndims);    
    m_mults[m_ndims-1] = 1;
    for (int i = m_ndims-2; i >= 0; --i)
        m_mults[i] = m_mults[i+1] * m_ranksperdim[i+1];
}

Layout::Layout(int argc, char **argv) :
    m_nranks(0), m_ndims(0)
{
    assert(false);
/*    
    vector<string> tokens;
    for (int i = 1; i < argc; ++i)
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 'm':
                tokenize("-m", argc, argv, i, tokens);
                m_ndims = tokens.size();
                m_ranksperdim = tokens2arr("-m", tokens);
                break;
            case 's':
                tokenize("-s", argc, argv, i, tokens);
                m_sizes = tokens2arr("-s", tokens);
                break;
            case 'e':
                tokenize("-e", argc, argv, i, tokens);
                m_enabled = tokens2arr("-e", tokens);
                break;
            case 'c':
                if (i+1 >= argc) {
                    fprintf(stderr, "No string after option -c\n");
                    usage(argc, argv);
                }
                ++i;
                m_niters = atoi(argv[i]);
                break;
            case 'p':
                if (i+1 >= argc) {
                    fprintf(stderr, "No string after option -p\n");
                    usage(argc, argv);
                }
                ++i;
                m_ppn = atoi(argv[i]);
                break;
            default:
                usage(argc, argv);
            }
        }
        else {
            usage(argc, argv);
        }
    if (m_ndims == 0)
        usage(argc, argv);
    if (m_sizes == 0) {
        m_sizes = new int[m_ndims];
        for (int i = 0; i < m_ndims; ++i)
            m_sizes[i] = 1024*1024;
    }
    if (m_enabled == 0) {
        m_enabled = new int[m_ndims];
        for (int i = 0; i < m_ndims; ++i)
            m_enabled[i] = 1;
    }
    m_nranks = 1;
    for (int i = 0; i < m_ndims; ++i)
        m_nranks *= m_ranksperdim[i];
    m_mults = new int[m_ndims];
    m_mults[m_ndims-1] = 1;
    for (int i = m_ndims-2; i >= 0; --i)
        m_mults[i] = m_mults[i+1] * m_ranksperdim[i+1];
*/        
}

void
Layout::prlayout()
{
    printf("%d dims, %d ranks, layout: ", m_ndims, m_nranks);
    prvals(m_ranksperdim);
//    printf("\nsizes: ");
//    prvals(m_sizes);
//    printf("\nenabled: ");
//    prvals(m_enabled);
    printf("\nmults: ");
    prvals(m_mults);
//    printf("\niterations %d\n", m_niters);
}

void
Layout::prsubs(const char *name, const unsigned *subs)
{
    printf("%s:", name);
    for (int i = 0; i < m_ndims; ++i)
        printf(" %d", subs[i]);
    printf("\n");
}

Layout::~Layout()
{
    free();
}
