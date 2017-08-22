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


// EXAMPLE 1: generic code, Benchmark class derectly inherits from base Benchmarks class.
// Suite code directly inherits from BenchmarkSuiteBase class.
// Registration is done by rather ugly direct register_elem() call from a constructor
class ExampleBenchmark_1 : public Benchmark 
{
    virtual void run(const scope_item &) { std::cout << "Hello from example 1"; }
};

class BenchmarkSuiteExample1 : public BenchmarkSuiteBase
{
    static BenchmarkSuiteExample1 *instance;
    public:
    BenchmarkSuiteExample1() { 
        if (instance == NULL) {
            instance = (BenchmarkSuiteExample1 *)0xffff;
            instance = new BenchmarkSuiteExample1(); 
            BenchmarkSuitesCollection::register_elem(instance);
        }
    }
    virtual ~BenchmarkSuiteExample1() {
        if (instance != NULL) {
            BenchmarkSuiteExample1 *to_delete;
            to_delete = instance;
            instance = NULL;
            delete to_delete;
        }
    }
    virtual void get_bench_list(std::set<std::string> &result, 
        BenchListFilter filter = ALL_BENCHMARKS) const 
    { 
        UNUSED(filter);
        result.insert("example_bench1");
    }
    virtual smart_ptr<Benchmark> create(const std::string &name) 
    {  
        if (name == "example_bench1") {
            return smart_ptr<Benchmark>(new ExampleBenchmark_1);
        }
        return smart_ptr<Benchmark>(NULL);
    }
    virtual const std::string get_name() const { return "example_suite1"; };
};

BenchmarkSuiteExample1 *BenchmarkSuiteExample1::instance;
namespace { BenchmarkSuiteExample1 bench_suite_ex1; }


// NOTE: Two following examples use BS_GENERIC template of BenchmarkSuite.
// One needs to take care when using it:
// 1. Put benchmark_suite.h include directive and all its usage in the namespace
// with unique name. It is required to avoid multiple occurence of static variables
// problem if BS_GENERIC is used by several suites as a base.
// 2. Put DECLARE_BENCHMARK_SUITE_STUFF right after include directive
// 3. Every benchmark you plan to put in suite must include DEFINE_INHERITED and
// DECLARE_INHERITED macros. 
// All other stuff is automated. Enjoy!
namespace example_suite2 {

    #include "benchmark_suite.h"
    DECLARE_BENCHMARK_SUITE_STUFF(BS_GENERIC, example_suite2)

    // EXAMPLE 2: code based on BenchmarkSuite<BS_EXAMPLE> specialization
    // Requires adding new BS_EXAMPLE field to enum
    class ExampleBenchmark_2 : public Benchmark {
        public:
        virtual void init() {
            VarLenScope *sc = new VarLenScope(0, 22);
            scope = sc;
        }
        virtual void run(const scope_item &item) { 
            cout << get_name() << ": Hello, world! size=" << item.len << endl;
        }
        DEFINE_INHERITED(ExampleBenchmark_2, BenchmarkSuite<BS_GENERIC>);
    };

    DECLARE_INHERITED(ExampleBenchmark_2, example2)

    // EXAMPLE 3: PingPong code based on BenchmarkSuite<BS_EXAMPLE> specialization
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
