#pragma once

#ifdef MPI1
extern "C" {
#include "IMB_benchmark.h"
#include "IMB_comm_info.h"
#include "IMB_prototypes.h"
}

//class OriginalBenchmarkSuite_MPI1 : public BenchmarkSuite<BS_MPI1> {
//    public:
//        static any &get_parameter(const std::string &key);
//};

#endif
