#pragma once
#include "benchmark_suite.h"
extern "C" {
#include "IMB_benchmark.h"
#include "IMB_comm_info.h"
#include "IMB_prototypes.h"
}


template <> void BenchmarkSuite<BS_MPI1>::declare_args(args_parser &parser) const;

template <> bool BenchmarkSuite<BS_MPI1>::prepare(const args_parser &parser, const set<string> &benchs);

template <> void BenchmarkSuite<BS_MPI1>::get_bench_list(std::set<std::string> &benchmarks, 
                                                         BenchmarkSuiteBase::BenchListFilter filter) const;

template <> const string BenchmarkSuite<BS_MPI1>::get_name() const { return "IMB-MPI1"; };

class OriginalBenchmarkSuite_MPI1 : public BenchmarkSuite<BS_MPI1> {
    public:
        using BenchmarkSuite<BS_MPI1>::prepare;
        using BenchmarkSuite<BS_MPI1>::declare_args;
        using BenchmarkSuite<BS_MPI1>::create;
        using BenchmarkSuite<BS_MPI1>::register_elem;
//        static void get_default_list(vector<string> &default_benchmarks);
        static void *get_internal_data_ptr(const std::string &key);
/*        
        void get_default_list(vector<string> &default_benchmarks) {
             for (map<string, const Benchmark*>::iterator it = pnames->begin();
                 it != pnames->end();
                 it++) {
                all_benchmarks.push_back(it->first);
            }
        }
        void get_full_list(vector<string> &all_benchmarks) {
            for (map<string, const Benchmark*>::iterator it = pnames->begin();
                 it != pnames->end();
                 it++) {
                all_benchmarks.push_back(it->first);
            }
        }
*/
};

