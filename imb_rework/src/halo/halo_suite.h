#pragma once

namespace ndim_halo_benchmark {
    #include "benchmark_suite.h"

    class HALOBenchmarkSuite : public BenchmarkSuite<BS_GENERIC> {
        public:
            static void *get_internal_data_ptr(const std::string &key, int i = 0);
    };
}

