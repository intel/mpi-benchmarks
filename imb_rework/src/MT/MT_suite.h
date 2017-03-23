#pragma once

template <> void BenchmarkSuite<BS_MT>::declare_args(args_parser &parser) const;

template <> bool BenchmarkSuite<BS_MT>::prepare(const args_parser &parser, const std::set<std::string> &benchs);

template <> void BenchmarkSuite<BS_MT>::finalize(const std::set<std::string> &benchs);


class MTBenchmarkSuite : public BenchmarkSuite<BS_MT> {
    public:
        using BenchmarkSuite<BS_MT>::prepare;
        using BenchmarkSuite<BS_MT>::declare_args;
        using BenchmarkSuite<BS_MT>::create;
        using BenchmarkSuite<BS_MT>::register_elem;
        static void *get_internal_data_ptr(const std::string &key, int i = 0);
};

