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

#define NUMBER_OF_NEIGHBORS 4

static int get_rank(int x, int y, int length_x, int length_y)
{
    x = (x + length_x) % length_x;
    y = (y + length_y) % length_y;
    return length_x * y + x;
}

void imb_p2p_stencil2d() {
    char *send_buffers[NUMBER_OF_NEIGHBORS];
    char *recv_buffers[NUMBER_OF_NEIGHBORS];
    int neighbours[NUMBER_OF_NEIGHBORS];
    size_t msg_size_index;
    int nranks = imb_p2p_config.nranks;
    int rank = imb_p2p_config.rank;
    int i, x, y;
    int length_x = 2;
    int length_y = 2;
    while ((length_x * length_y) < nranks) {
        length_x++;
        length_y++;
    }
    if ((length_x * length_y) > nranks) {
        while ((length_y > 1) && (nranks % length_y)) {
            length_y--;
        }
        length_x = nranks / length_y;
    }
    if ((length_x < 2) || (length_y < 2) || ((length_x * length_y) != nranks)) {
        if (rank == 0) {
            fprintf(unit, "\n");
            fprintf(unit, "# !! Benchmark %s is invalid for %d processes !!\n", IMB_P2P_STENCIL2D, nranks);
            fflush(unit);
        }
        return;
    }
    y = rank / length_x;
    x = rank % length_x;
    neighbours[0] = get_rank(x - 1, y, length_x, length_y);
    neighbours[1] = get_rank(x + 1, y, length_x, length_y);
    neighbours[2] = get_rank(x, y - 1, length_x, length_y);
    neighbours[3] = get_rank(x, y + 1, length_x, length_y);
    if (rank == 0) {
        fprintf(unit, "\n");
        fprintf(unit, "#----------------------------------------------------------------\n");
        fprintf(unit, "# Benchmarking %s (%d x %d)\n", IMB_P2P_STENCIL2D, length_y, length_x);
        fprintf(unit, "# #processes = %d\n", imb_p2p_config.nranks);
        fprintf(unit, "#----------------------------------------------------------------\n");
        fflush(unit);
        fprintf(unit, " %12s %12s %12s %12s %12s\n", "#bytes", "#repetitions", "t[usec]", "Mbytes/sec", "Msg/sec");
        fflush(unit);
    }
    for (i = 0; i < NUMBER_OF_NEIGHBORS; i++) {
        send_buffers[i] = (char *)imb_p2p_alloc_mem(imb_p2p_config.messages.max_size);
    }
    for (i = 0; i < NUMBER_OF_NEIGHBORS; i++) {
        recv_buffers[i] = (char *)imb_p2p_alloc_mem(imb_p2p_config.messages.max_size);
    }
    for (i = 0; i < NUMBER_OF_NEIGHBORS; i++) {
        memset(send_buffers[i], rank, imb_p2p_config.messages.max_size);
    }
    for (i = 0; i < NUMBER_OF_NEIGHBORS; i++) {
        memset(recv_buffers[i], rank, imb_p2p_config.messages.max_size);
    }
    for (msg_size_index = 0; msg_size_index < imb_p2p_config.messages.length; msg_size_index++) {
        MPI_Request requests[NUMBER_OF_NEIGHBORS * 2];
        size_t size = imb_p2p_config.messages.array[msg_size_index];
        size_t iteration, number_of_iterations, number_of_warm_up_iterations;
        double time;
        size_t number_of_messages = 0;
        get_iters(size, &number_of_iterations, &number_of_warm_up_iterations);
        imb_p2p_pause();
        imb_p2p_barrier(MPI_COMM_WORLD);
        for (iteration = 0; iteration < number_of_warm_up_iterations; iteration++) {
            for (i = 0; i < NUMBER_OF_NEIGHBORS; i++) {
                touch_send_buff(size, send_buffers[i]);
            }
            for (i = 0; i < NUMBER_OF_NEIGHBORS; i++) {
                MPI_Irecv(recv_buffers[i], size, MPI_BYTE, neighbours[i], 0, MPI_COMM_WORLD, &requests[i]);
            }
            for (i = 0; i < NUMBER_OF_NEIGHBORS; i++) {
                MPI_Isend(send_buffers[i], size, MPI_BYTE, neighbours[i], 0, MPI_COMM_WORLD, &requests[NUMBER_OF_NEIGHBORS + i]);
            }
            MPI_Waitall((NUMBER_OF_NEIGHBORS * 2), requests, MPI_STATUSES_IGNORE);
            for (i = 0; i < NUMBER_OF_NEIGHBORS; i++) {
                touch_recv_buff(size, recv_buffers[i]);
            }
        }
        imb_p2p_barrier(MPI_COMM_WORLD);
        time = MPI_Wtime();
        for (iteration = 0; iteration < number_of_iterations; iteration++) {
            for (i = 0; i < NUMBER_OF_NEIGHBORS; i++) {
                touch_send_buff(size, send_buffers[i]);
            }
            for (i = 0; i < NUMBER_OF_NEIGHBORS; i++) {
                MPI_Irecv(recv_buffers[i], size, MPI_BYTE, neighbours[i], 0, MPI_COMM_WORLD, &requests[i]);
            }
            for (i = 0; i < NUMBER_OF_NEIGHBORS; i++) {
                MPI_Isend(send_buffers[i], size, MPI_BYTE, neighbours[i], 0, MPI_COMM_WORLD, &requests[NUMBER_OF_NEIGHBORS + i]);
            }
            MPI_Waitall((NUMBER_OF_NEIGHBORS * 2), requests, MPI_STATUSES_IGNORE);
            for (i = 0; i < NUMBER_OF_NEIGHBORS; i++) {
                touch_recv_buff(size, recv_buffers[i]);
            }
            number_of_messages += (NUMBER_OF_NEIGHBORS * 2);
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
    for (i = 0; i < NUMBER_OF_NEIGHBORS; i++) {
        imb_p2p_free_mem(send_buffers[i]);
        imb_p2p_free_mem(recv_buffers[i]);
    }
}
