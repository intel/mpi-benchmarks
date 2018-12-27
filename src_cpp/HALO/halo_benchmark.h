/*****************************************************************************
 *                                                                           *
 * Copyright 2016-2019 Intel Corporation.                                    *
 *                                                                           *
 *****************************************************************************

This code is covered by the Community Source License (CPL), version
1.0 as published by IBM and reproduced in the file "license.txt" in the
"license" subdirectory. Redistribution in source and binary form, with
or without modification, is permitted ONLY within the regulations
contained in above mentioned license.

Use of the name and trademark "Intel(R) MPI Benchmarks" is allowed ONLY
within the regulations of the "License for Use of "Intel(R) MPI
Benchmarks" Name and Trademark" as reproduced in the file
"use-of-trademark-license.txt" in the "license" subdirectory.

THE PROGRAM IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT
LIMITATION, ANY WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT,
MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Each Recipient is
solely responsible for determining the appropriateness of using and
distributing the Program and assumes all risks associated with its
exercise of rights under this Agreement, including but not limited to
the risks and costs of program errors, compliance with applicable
laws, damage to or loss of data, programs or equipment, and
unavailability or interruption of operations.

EXCEPT AS EXPRESSLY SET FORTH IN THIS AGREEMENT, NEITHER RECIPIENT NOR
ANY CONTRIBUTORS SHALL HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING
WITHOUT LIMITATION LOST PROFITS), HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OR
DISTRIBUTION OF THE PROGRAM OR THE EXERCISE OF ANY RIGHTS GRANTED
HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF
YOUR JURISDICTION. It is licensee's responsibility to comply with any
export regulations applicable in licensee's jurisdiction. Under
CURRENT U.S. export regulations this software is eligible for export
from the U.S. and can be downloaded by or otherwise exported or
reexported worldwide EXCEPT to U.S. embargoed destinations which
include Cuba, Iraq, Libya, North Korea, Iran, Syria, Sudan,
Afghanistan and any other country to which the U.S. has embargoed
goods and services.

 ***************************************************************************
*/

#pragma once
#include <mpi.h>

namespace ndim_halo_benchmark {

#include "MT_benchmark.h"

template <class bs>
class HaloBenchmark : public Benchmark { 
    public:    
    MPI_Datatype datatype;
    size_t datatype_size;
    std::vector<thread_local_data_t> input;
    std::vector<int> count;
    int mode_multiple;
    int num_threads;
    int malloc_align;
    double time_avg, time_min, time_max;

    int rank, nranks;
    int ndims, required_nranks;
    std::vector<std::vector<std::vector<void *> > > buffs;
    std::vector<std::vector<int> > partner;
    std::vector<int> ranksperdim;
    std::vector<int> mults;
    enum { UP=0, DN }; static const int ndirs = 2;
    enum { SEND=0, RECV }; static const int nsr = 2;

    // linearize
    int substorank(const std::vector<unsigned int> &subs)
    {
        int rank = 0;
        // last subscript varies fastest
        for (int i = 0; i < ndims; ++i)
            rank += mults[i] * subs[i];
        return rank;
    }
    // delinearize
    void ranktosubs(int rank, std::vector<unsigned int> &subs)
    {
        int rem = rank;
        for (int i = 0; i < ndims; ++i) {
            int sub = rem / mults[i];
            rem %= mults[i];
            subs[i] = sub;
        }
    }

    virtual void init() {
        GET_GLOBAL(vector<thread_local_data_t>, input);
        GET_GLOBAL(int, mode_multiple);
        GET_GLOBAL(int, num_threads);
        GET_GLOBAL(int, malloc_align);
        GET_GLOBAL(MPI_Datatype, datatype);
        GET_GLOBAL(std::vector<int>, count);

        VarLenScope *sc = new VarLenScope(count);
        scope = sc;

        size_t maxlen = scope->get_max_len();
        int idts;
        MPI_Type_size(datatype, &idts);
        size_t datatype_size = idts;
        size_t buf_size = maxlen * datatype_size;

        GET_GLOBAL(int, rank);
        GET_GLOBAL(int, nranks);
        GET_GLOBAL(int, ndims);
        GET_GLOBAL(int, required_nranks);
        GET_GLOBAL(std::vector<int>, ranksperdim);
        GET_GLOBAL(std::vector<int>, mults);

        if (rank >= required_nranks)
            return;
        std::vector<unsigned int> mysubs;
        mysubs.resize(ndims);
        ranktosubs(rank, mysubs);

        buffs.resize(ndims);
        for (int i = 0; i < ndims; i++) {
            buffs[i].resize(ndirs);
            for (int j = 0; j < ndirs; j++) {
                buffs[i][j].resize(nsr);
            }
        }

        static AlignedAllocator<char> allocator(malloc_align);

        for (int i = 0; i < ndims; ++i)
            for (int j = UP; j <= DN; ++j) 
                for (int k = SEND; k <= RECV; ++k) {
                    buffs[i][j][k] = allocator.Alloc(buf_size);
                }

        partner.resize(ndims);
        for (int i = 0; i < ndims; i++) {
            partner[i].resize(ndirs);
        }

        // construct the partners
        for (int dim = 0; dim < ndims; ++dim) {
            std::vector<unsigned int> partnersubs(ndims);
            for (int i = 0; i < ndims; ++i) partnersubs[i] = mysubs[i];
            partnersubs[dim] = (mysubs[dim]+1)%ranksperdim[dim];
            partner[dim][UP] = substorank(partnersubs);
            partnersubs[dim] = (ranksperdim[dim]+mysubs[dim]-1)%ranksperdim[dim];
            partner[dim][DN] = substorank(partnersubs);

        }
    }
    struct chunk_t {
        size_t offset;
        size_t count;
    };
    void split_into_chunks(int count, int num, std::vector<chunk_t> &chunks) {
        size_t nparts = (count > num) ? num : count;

        size_t base = count / nparts;
        size_t rest = count % nparts;
        size_t base_off = 0;
        chunks.resize(nparts);
        for (size_t i = 0; i < nparts; i++) {
            chunks[i].offset = base_off; 
            base_off += (chunks[i].count = base + (i<rest?1:0)); 
        }
    }
    virtual void run(const scope_item &item) {
        static int ninvocations = 0;
        double t, tavg = 0, tmin = 1e6, tmax = 0;
        int nresults = 0;
        double transferred_bytes = 0;
  

        int idts;
        MPI_Type_size(datatype, &idts);
        size_t datatype_size = (size_t)idts;

        int actual_nthreads = 1;
        if (mode_multiple) {
            std::vector<chunk_t> chunks;
            split_into_chunks(item.len, num_threads, chunks);
            // NOTE: actual_nthreads might appear smaller than num_threads for small message
            // sizes!
            actual_nthreads = chunks.size();
            
        #pragma omp parallel default(shared) num_threads(actual_nthreads)
            {
                double t_mp;
                int result;
                size_t total_count;
                run_instance(&input[omp_get_thread_num()], chunks[omp_get_thread_num()], t_mp, result, total_count);
            #pragma omp critical
                {
                    tmax = max(tmax, t_mp);
                    tmin = min(tmin, t_mp);
                    tavg = tavg + t_mp;
                    nresults += result;
                    transferred_bytes += (double)(total_count * datatype_size);
                }
            }
        } else {
            chunk_t the_only_chunk = { 0, item.len };
            size_t total_count;
            run_instance(&input[0], the_only_chunk, t, nresults, total_count);
            tavg = tmax = tmin = t;
            transferred_bytes = (double)(total_count * datatype_size);
        }
        MPI_Allreduce(&tavg, &time_avg, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(&tmin, &time_min, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
        MPI_Allreduce(&tmax, &time_max, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
        MPI_Allreduce(MPI_IN_PLACE, &nresults, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        if (nresults) {
            time_avg /= (double)nresults;
        }
        time_avg /= (double)input[0].repeat;
        time_min /= (double)input[0].repeat;
        time_max /= (double)input[0].repeat;
        if (rank == 0) {
            if (nresults) {
                if (ninvocations++ == 0) {
                    std::cout << std::endl;
                    std::cout << "# Benchmarking " << get_name() << 
                        " (processes: " << nresults / actual_nthreads << "; threads: " << actual_nthreads <<
                        "; dimensions: " << ndims << ")" << std::endl;
                }
                int ntimes = transferred_bytes / (item.len * datatype_size);
                std::cout << item.len * datatype_size << "(x" << ntimes << ")(bytes) " << time_avg / 1e-6 << "(usec) " << transferred_bytes / time_avg / 1e6 << "(Mb/s)" << std::endl;
                
            }
        }
    }
    virtual void run_instance(thread_local_data_t *input, chunk_t chunk, double &t, int &result, size_t &total_count) {
        MPI_Comm comm = input->comm;
        t = 0;
        result = 0;
        if (rank >= required_nranks)
            return;

        int idts;
        MPI_Type_size(datatype, &idts);
        size_t datatype_size = (size_t)idts;
        
#if 0        
        MPI_Status status;
        t = MPI_Wtime();
        for (int iter = 0; iter < parent::input->repeat; ++iter) {
            for (int i = 0; i < ndims; ++i) {
                printf("MPI_Sendrecv(size=%u, send_to=%d, recv_from=%d);\n",
                    count, partner[i][UP], partner[i][DN]);
                MPI_Sendrecv(buffs[i][UP][SEND], count, parent::datatype, partner[i][UP], 1,
                             buffs[i][UP][RECV], count, parent::datatype, partner[i][DN], 1,
                             MPI_COMM_WORLD, &status);
                printf("MPI_Sendrecv(size=%u, send_to=%d, recv_from=%d);\n",
                    count, partner[i][DN], partner[i][UP]);
                MPI_Sendrecv(buffs[i][DN][SEND], count, parent::datatype, partner[i][DN], 1,
                             buffs[i][DN][RECV], count, parent::datatype, partner[i][UP], 1,
                             MPI_COMM_WORLD, &status);
                totsize += 4 * count;
            }
        }
#else
        const int maxreqs = 4 * ndims;
        int nreqs = 0;
        std::vector<MPI_Request> reqs(maxreqs);
        for (int iter = 0; iter < input->warmup + input->repeat; ++iter) {
            if (iter == input->warmup) {
                t = MPI_Wtime();
            }
            nreqs = 0;
            for (int i = 0; i < ndims; ++i) {
                if (ranksperdim[i] == 1) 
                    continue;
                MPI_Irecv((char *)buffs[i][UP][RECV] + chunk.offset * datatype_size, chunk.count, datatype, 
                          partner[i][DN], 1, comm, &reqs[nreqs++]);
//                printf(">> Irecv: %d->%d count=%d\n", partner[i][DN], rank, chunk.count);
            
                MPI_Irecv((char *)buffs[i][DN][RECV] + chunk.offset * datatype_size, chunk.count, datatype, 
                          partner[i][UP], 1, comm, &reqs[nreqs++]);
//                printf(">> Irecv: %d->%d\n", partner[i][UP], rank);
            }
            for (int i = 0; i < ndims; ++i) {
                if (ranksperdim[i] == 1)
                    continue;
                MPI_Isend((char *)buffs[i][UP][SEND] + chunk.offset * datatype_size, chunk.count, datatype, 
                          partner[i][UP], 1, comm, &reqs[nreqs++]);
//                printf(">> Isend: %d->%d\n", rank, partner[i][UP]);

                MPI_Isend((char *)buffs[i][DN][SEND] + chunk.offset * datatype_size, chunk.count, datatype, 
                          partner[i][DN], 1, comm, &reqs[nreqs++]);
//                printf(">> Isend: %d->%d\n", rank, partner[i][DN]);
            }
            assert(nreqs <= maxreqs);
            result = 1;
            total_count = nreqs * 
            MPI_Waitall(nreqs, &reqs[0], MPI_STATUSES_IGNORE);
        }
        result = 1;
        total_count = (size_t)nreqs * (size_t)chunk.count;
#endif        
        t = MPI_Wtime() - t;
    }
    virtual void finalize() {
        if (rank >= required_nranks)
            return;
    }
    DEFINE_INHERITED(HaloBenchmark<bs>, bs);
};

}

