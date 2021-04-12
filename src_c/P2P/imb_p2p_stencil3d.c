/*
*****************************************************************************
*                                                                           *
* Copyright Intel Corporation.                                              *
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
*/

#include "imb_p2p.h"

#define NUMBER_OF_NEIGHBORS 6

static int get_rank(int x, int y, int z, int length_x, int length_y, int length_z)
{
    x = (x + length_x) % length_x;
    y = (y + length_y) % length_y;
    z = (z + length_z) % length_z;
    return length_x * length_y * z + length_x * y + x;
}

void imb_p2p_stencil3d() {
    char *send_buffers[NUMBER_OF_NEIGHBORS];
    char *recv_buffers[NUMBER_OF_NEIGHBORS];
    int neighbours[NUMBER_OF_NEIGHBORS];
    size_t msg_size_index;
    int nranks = imb_p2p_config.nranks;
    int rank = imb_p2p_config.rank;
    int i, x, y, z;
    int length_x = 2;
    int length_y = 2;
    int length_z = 2;
    while ((length_x * length_y * length_z) < nranks) {
        length_x++;
        length_y++;
        length_z++;
    }
    while ((length_x * length_y * length_z) > nranks) {
        int n, lx, ly;
        length_z--;
        while ((length_z > 1) && (nranks % length_z)) {
            length_z--;
        }
        n = nranks / length_z;
        lx = 2;
        ly = 2;
        while ((lx * ly) < n) {
            lx++;
            ly++;
        }
        if ((lx * ly) > n) {
            while ((ly > 1) && (n % ly)) {
                ly--;
            }
            lx = n / ly;
        }
        if (ly >= length_z) {
            length_x = lx;
            length_y = ly;
        }
    }
    if ((length_x < 2) || (length_y < 2) || (length_z < 2) || ((length_x * length_y * length_z) != nranks)) {
        if (rank == 0) {
            fprintf(unit, "\n");
            fprintf(unit, "# !! Benchmark %s is invalid for %d processes !!\n", IMB_P2P_STENCIL3D, nranks);
            fflush(unit);
        }
        return;
    }
    z = (rank / (length_x * length_y));
    y = (rank - length_x * length_y * z) / length_x;
    x = (rank - length_x * length_y * z) % length_x;
    neighbours[0] = get_rank(x - 1, y, z, length_x, length_y, length_z);
    neighbours[1] = get_rank(x + 1, y, z, length_x, length_y, length_z);
    neighbours[2] = get_rank(x, y - 1, z, length_x, length_y, length_z);
    neighbours[3] = get_rank(x, y + 1, z, length_x, length_y, length_z);
    neighbours[4] = get_rank(x, y, z - 1, length_x, length_y, length_z);
    neighbours[5] = get_rank(x, y, z + 1, length_x, length_y, length_z);
    if (rank == 0) {
        fprintf(unit, "\n");
        fprintf(unit, "#----------------------------------------------------------------\n");
        fprintf(unit, "# Benchmarking %s (%d x %d x %d)\n", IMB_P2P_STENCIL3D, length_z, length_y, length_x);
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
