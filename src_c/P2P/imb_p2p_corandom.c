/****************************************************************************
*                                                                           *
* Copyright (C) 2020 Intel Corporation                                      *
*                                                                           *
*****************************************************************************

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ***************************************************************************/

#include "imb_p2p.h"

void imb_p2p_corandom() {
    char *s_buffer;
    char *r_buffer;
    int *random_ring;
    size_t msg_size_index;
    int nranks = imb_p2p_config.nranks;
    int rank = imb_p2p_config.rank;
    if (nranks < 2) {
        if (rank == 0) {
            fprintf(unit, "\n");
            fprintf(unit, "# !! Benchmark %s is invalid for %d processes !!\n", IMB_P2P_CORANDOM, nranks);
            fflush(unit);
        }
        return;
    }
    if (rank == 0) {
        imb_p2p_print_benchmark_header(IMB_P2P_CORANDOM);
        fprintf(unit, " %12s %12s %12s %12s %12s\n", "#bytes", "#repetitions", "t[usec]", "Mbytes/sec", "Msg/sec");
        fflush(unit);
    }
    s_buffer = (char *)imb_p2p_alloc_mem(imb_p2p_config.messages.max_size);
    r_buffer = (char *)imb_p2p_alloc_mem(imb_p2p_config.messages.max_size);
    random_ring = (int *)imb_p2p_alloc_mem(nranks * sizeof(int));
    memset(s_buffer, rank, imb_p2p_config.messages.max_size);
    memset(r_buffer, rank, imb_p2p_config.messages.max_size);
    for (msg_size_index = 0; msg_size_index < imb_p2p_config.messages.length; msg_size_index++) {
        size_t size = imb_p2p_config.messages.array[msg_size_index];
        size_t iteration, number_of_iterations, number_of_warm_up_iterations;
        double time;
        int rank0, rank1;
        size_t number_of_messages = 0;
        uint64_t random_seed = 0;
        MPI_Request request;
        int i;
        get_iters(size, &number_of_iterations, &number_of_warm_up_iterations);
        for (i = 0; i < nranks; i++) {
            random_ring[i] = i;
        }
        imb_p2p_pause();
        imb_p2p_barrier(MPI_COMM_WORLD);
        for (iteration = 0; iteration < number_of_warm_up_iterations; iteration++) {
            for (i = 0; i < nranks; i++) {
                int j = get_next_random(&random_seed) % nranks;
                int r = random_ring[i];
                random_ring[i] = random_ring[j];
                random_ring[j] = r;
            }
            i = 0;
            while (random_ring[i] != rank) {
                i++;
            }
            rank0 = random_ring[(i + nranks - 1) % nranks];
            rank1 = random_ring[(i + 1) % nranks];
            MPI_Irecv(r_buffer, size, MPI_BYTE, rank0, 0, MPI_COMM_WORLD, &request);
            touch_send_buff(size, s_buffer);
            MPI_Send(s_buffer, size, MPI_BYTE, rank1, 0, MPI_COMM_WORLD);
            MPI_Wait(&request, MPI_STATUSES_IGNORE);
            touch_recv_buff(size, r_buffer);
        }
        for (i = 0; i < nranks; i++) {
            random_ring[i] = i;
        }
        random_seed = 0;
        imb_p2p_barrier(MPI_COMM_WORLD);
        time = MPI_Wtime();
        for (iteration = 0; iteration < number_of_iterations; iteration++) {
            for (i = 0; i < nranks; i++) {
                int j = get_next_random(&random_seed) % nranks;
                int r = random_ring[i];
                random_ring[i] = random_ring[j];
                random_ring[j] = r;
            }
            i = 0;
            while (random_ring[i] != rank) {
                i++;
            }
            rank0 = random_ring[(i + nranks - 1) % nranks];
            rank1 = random_ring[(i + 1) % nranks];
            MPI_Irecv(r_buffer, size, MPI_BYTE, rank0, 0, MPI_COMM_WORLD, &request);
            touch_send_buff(size, s_buffer);
            MPI_Send(s_buffer, size, MPI_BYTE, rank1, 0, MPI_COMM_WORLD);
            MPI_Wait(&request, MPI_STATUSES_IGNORE);
            touch_recv_buff(size, r_buffer);
            number_of_messages += 2;
        }
        time = MPI_Wtime() - time;
        imb_p2p_pause();
        imb_p2p_barrier(MPI_COMM_WORLD);
        if (rank) {
            imb_p2p_report_t report;
            report.time = time;
            report.number_of_messages = number_of_messages;
            MPI_Send(&report, sizeof(imb_p2p_report_t), MPI_BYTE, 0, 1, MPI_COMM_WORLD);
        } else {
            double bandwidth, message_rate, latency;
            double max_time = time;
            double aggregate_time = time;
            size_t aggregate_number_of_messages = number_of_messages;
            int source_rank;
            for (source_rank = 1; source_rank < nranks; source_rank++) {
                imb_p2p_report_t report;
                MPI_Recv(&report, sizeof(imb_p2p_report_t), MPI_BYTE, source_rank, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                aggregate_time += report.time;
                aggregate_number_of_messages += report.number_of_messages;
                if (max_time < report.time) {
                    max_time = report.time;
                }
            }
            aggregate_number_of_messages /= 2;
            message_rate = (aggregate_number_of_messages / max_time);
            bandwidth = ((aggregate_number_of_messages * size) / (1000000.0 * max_time));
            latency = (1000000.0 * aggregate_time) / aggregate_number_of_messages;
            fprintf(unit, " %12" PRIu64 " %12" PRIu64 " %12.2f %12.2f %12.0f\n", size, number_of_iterations, latency, bandwidth, message_rate);
            fflush(unit);
        }
    }
    imb_p2p_free_mem(s_buffer);
    imb_p2p_free_mem(r_buffer);
    imb_p2p_free_mem(random_ring);
}
