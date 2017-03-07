#pragma once
#include <mpi.h>
#include <iostream>

#include "benchmark.h"
#include "benchmark_suite.h"

using namespace std;

class ExampleBenchmark_1 : public Benchmark {
    static const char *name;
    public:
    virtual void init() {
        VarLenLogScope *sc = new VarLenLogScope(0, 22);
        scope = sc;
    }
    virtual void run(const std::pair<int, int> &p) { 
        cout << "Hello, world! " << p.second << endl;
    }
    DEFINE_INHERITED(ExampleBenchmark_1, BenchmarkSuite<BS_EXAMPLE>);
};

class ExampleBenchmark_2 : public Benchmark {
    static const char *name;
    std::map<int, double> results;
    char *sbuf, *rbuf;
    int np, rank;
    public:
    virtual void init() {
        VarLenLogScope *sc = new VarLenLogScope(0, 22);
        scope = sc;
        rbuf = (char *)malloc(1 << 22);
        sbuf = (char *)malloc(1 << 22);
    }
    virtual void run(const std::pair<int, int> &p) { 
        MPI_Comm_size(MPI_COMM_WORLD, &np);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        if (np < 2) {
            std::cout << "ExampleBenchmark_2: two or more ranks required" << std::endl;
            return;
        }
        MPI_Status stat;
        double t1 = 0, t2 = 0, time = 0;
        const int tag = 1;
        const int ncycles = 1024;
        if (rank == 0) {
            t1 = MPI_Wtime();
            for(int i = 0; i < ncycles; i++) {
                MPI_Send((char*)sbuf, p.second, MPI_BYTE, 1, tag, MPI_COMM_WORLD);
                MPI_Recv((char*)rbuf, p.second, MPI_BYTE, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
            }
            t2 = MPI_Wtime();
            time = (t2 - t1) / ncycles;
        } else if (rank == 1) {
            t1 = MPI_Wtime();
            for(int i = 0; i < ncycles; i++) {
                MPI_Recv((char*)rbuf, p.second, MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
                MPI_Send((char*)sbuf, p.second, MPI_BYTE, 0, tag, MPI_COMM_WORLD);
            }
            t2 = MPI_Wtime();
            time = (t2 - t1) / ncycles;
        } 
        MPI_Barrier(MPI_COMM_WORLD);
        results[p.second] = time;
    }
    virtual void finalize() { 
        if (rank == 0) {
            for (std::map<int, double>::iterator it = results.begin();
                    it != results.end(); ++it) {
                std::cout << "ExampleBenchmark_2: " << "len=" << it->first << " time=" << it->second << std::endl; 
            }
        }
    }
    ~ExampleBenchmark_2() {
        free(rbuf);
        free(sbuf);
    }
    DEFINE_INHERITED(ExampleBenchmark_2, BenchmarkSuite<BS_EXAMPLE>);
};

