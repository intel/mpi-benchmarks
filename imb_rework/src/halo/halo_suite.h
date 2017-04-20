#pragma once

template <> void BenchmarkSuite<BS_HALO>::declare_args(args_parser &parser) const;

template <> bool BenchmarkSuite<BS_HALO>::prepare(const args_parser &parser, const std::set<std::string> &benchs);

template <> void BenchmarkSuite<BS_HALO>::finalize(const std::set<std::string> &benchs);


class HALOBenchmarkSuite : public BenchmarkSuite<BS_HALO> {
    public:
        using BenchmarkSuite<BS_HALO>::prepare;
        using BenchmarkSuite<BS_HALO>::declare_args;
        using BenchmarkSuite<BS_HALO>::create;
        using BenchmarkSuite<BS_HALO>::register_elem;
        static void *get_internal_data_ptr(const std::string &key, int i = 0);
};

