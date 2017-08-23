#include <mpi.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>

#include "benchmark.h"
#include "benchmark_suites_collection.h"
#include "scope.h"
#include "utils.h"

using namespace std;

namespace example_suite3 {

    #include "benchmark_suite.h"
    DECLARE_BENCHMARK_SUITE_STUFF(BS_GENERIC, example_suite3)

    // EXAMPLE 3: PingPong code based on BenchmarkSuite<BS_GENERIC> specialization
    // - allocation of buffers added to init() overloaded virtual function
    // - finalize() overloaded virtual function to do deallocation and
    // some results output 
    class ExampleBenchmark_3 : public Benchmark {
        std::map<int, double> results;
        char *sbuf, *rbuf;
        int np, rank;
        public:
        virtual void init() {
            VarLenScope *sc = new VarLenScope(0, 22);
            scope = sc;
            rbuf = (char *)malloc(1 << 22);
            sbuf = (char *)malloc(1 << 22);
        }
        virtual void run(const scope_item &item) { 
            MPI_Comm_size(MPI_COMM_WORLD, &np);
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            if (np < 2) {
                std::cout << get_name() << ": two or more ranks required" << std::endl;
                return;
            }
            MPI_Status stat;
            double t1 = 0, t2 = 0, time = 0;
            const int tag = 1;
            const int ncycles = 1024;
            if (rank == 0) {
                t1 = MPI_Wtime();
                for(int i = 0; i < ncycles; i++) {
                    MPI_Send((char*)sbuf, item.len, MPI_BYTE, 1, tag, MPI_COMM_WORLD);
                    MPI_Recv((char*)rbuf, item.len, MPI_BYTE, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
                }
                t2 = MPI_Wtime();
                time = (t2 - t1) / ncycles;
            } else if (rank == 1) {
                t1 = MPI_Wtime();
                for(int i = 0; i < ncycles; i++) {
                    MPI_Recv((char*)rbuf, item.len, MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
                    MPI_Send((char*)sbuf, item.len, MPI_BYTE, 0, tag, MPI_COMM_WORLD);
                }
                t2 = MPI_Wtime();
                time = (t2 - t1) / ncycles;
            } 
            MPI_Barrier(MPI_COMM_WORLD);
            results[item.len] = time;
        }
        virtual void finalize() { 
            if (rank == 0) {
                for (std::map<int, double>::iterator it = results.begin();
                        it != results.end(); ++it) {
                    std::cout << get_name() << ": " << "len=" << it->first << " time=" << it->second << std::endl; 
                }
            }
        }
        virtual ~ExampleBenchmark_3() {
            free(rbuf);
            free(sbuf);
        }
        DEFINE_INHERITED(ExampleBenchmark_3, BenchmarkSuite<BS_GENERIC>);
    };

    DECLARE_INHERITED(ExampleBenchmark_3, example3)
}
