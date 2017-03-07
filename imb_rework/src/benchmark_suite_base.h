#pragma once

#include "smart_ptr.h"
#include "benchmark.h"

class args_parser;
class Benchmark;

struct BenchmarkSuiteBase {
    enum BenchListFilter { ALL_BENCHMARKS, DEFAULT_BENCHMARKS };
    virtual void declare_args(args_parser &parser) const {};
    virtual bool prepare(const args_parser &parser, const std::set<std::string> &benchs) { return true; }
    virtual void finalize(const std::set<std::string> &benchs) { }
    virtual void get_bench_list(std::set<std::string> &benchs, BenchListFilter filter = ALL_BENCHMARKS) const {}
    virtual smart_ptr<Benchmark> create(const std::string &name) { return smart_ptr<Benchmark>(); }
    virtual const std::string get_name() const = 0;
};

