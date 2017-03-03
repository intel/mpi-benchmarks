#pragma once

#ifdef MPI1

#include "benchmark_suite.h"
extern "C" {
#include "IMB_benchmark.h"
#include "IMB_comm_info.h"
#include "IMB_prototypes.h"
}


template <> void BenchmarkSuite<BS_MPI1>::declare_args(args_parser &parser) const;

template <> bool BenchmarkSuite<BS_MPI1>::prepare(const args_parser &parser, const std::set<std::string> &benchs);

template <> void BenchmarkSuite<BS_MPI1>::get_bench_list(std::set<std::string> &benchmarks, 
                                                         BenchmarkSuiteBase::BenchListFilter filter) const;

template <> const std::string BenchmarkSuite<BS_MPI1>::get_name() const { return "IMB-MPI1"; };

class OriginalBenchmarkSuite_MPI1 : public BenchmarkSuite<BS_MPI1> {
    public:
        using BenchmarkSuite<BS_MPI1>::prepare;
        using BenchmarkSuite<BS_MPI1>::declare_args;
        using BenchmarkSuite<BS_MPI1>::create;
        using BenchmarkSuite<BS_MPI1>::register_elem;
        static void *get_internal_data_ptr(const std::string &key);
};

#endif
