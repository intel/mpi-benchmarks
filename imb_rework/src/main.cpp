#include <iostream>
#include <stdexcept>

#include "benchmark_suite.h"


int main()
{
    try {
        shared_ptr<Benchmark> b1 = BenchmarkSuite<BS_MPI1>::create("pingpong"); 
        shared_ptr<Benchmark> b2 = BenchmarkSuite<BS_MPI1>::create("pingping");
        cout << b1->get_name() << endl;
        b1->run();
        cout << b2->get_name() << endl;
        b2->run();
    }
    catch(std::invalid_argument &ex) {
        cout << "Not found benchmark: " << ex.what() << endl;
    }
    return 0;
}
