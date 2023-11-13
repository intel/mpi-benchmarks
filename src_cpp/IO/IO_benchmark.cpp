/****************************************************************************
*                                                                           *
* Copyright (C) 2023 Intel Corporation                                      *
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

#include <mpi.h>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <iostream>
#include "smart_ptr.h"
#include "benchmark_suites_collection.h"
#include "utils.h"
#include "benchmark_suite.h"
#include "original_benchmark.h"

extern "C" {
#include "IMB_benchmark.h"
#include "IMB_comm_info.h"
#include "IMB_prototypes.h"
}

#include "helper_IMB_functions.h"

using namespace std;

#define BENCHMARK(BMRK_FN, BMRK_NAME) \
template<> smart_ptr<Bmark_descr> OriginalBenchmark<BenchmarkSuite<BS_IO>, BMRK_FN>::descr = NULL; \
DECLARE_INHERITED_TEMPLATE(GLUE_TYPENAME(OriginalBenchmark<BenchmarkSuite<BS_IO>, BMRK_FN>), BMRK_NAME) \
template class OriginalBenchmark<BenchmarkSuite<BS_IO>, BMRK_FN>; \
template<> bool OriginalBenchmark<BenchmarkSuite<BS_IO>, BMRK_FN>::init_description() 

BENCHMARK(IMB_write_indv, S_Write_Indv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(PRIVATE);
    descr->flags.insert(PUT);
    return true;
}

void IMB_s_iwrite_indv(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_write_indv(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_s_iwrite_indv, S_IWrite_Indv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_2);
    descr->flags.insert(PRIVATE);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(PUT);
    return true;
}

BENCHMARK(IMB_write_expl, S_Write_Expl)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(PRIVATE);
    descr->flags.insert(PUT);
    return true;
}

void IMB_s_iwrite_expl(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_write_expl(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_s_iwrite_expl, S_IWrite_Expl)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_2);
    descr->flags.insert(PRIVATE);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(PUT);
    return true;
}

void IMB_p_write_indv(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_write_indv(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_p_write_indv, P_Write_Indv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(PARALLEL_TRANSFER);
    descr->flags.insert(INDV_BLOCK);
    descr->flags.insert(PUT);
    return true;
}

void IMB_p_iwrite_indv(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_write_indv(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_p_iwrite_indv, P_IWrite_Indv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(PARALLEL_TRANSFER);
    descr->flags.insert(INDV_BLOCK);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_2);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(PUT);
    return true;
}

void IMB_p_write_shared(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_write_shared(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_p_write_shared, P_Write_Shared)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(PARALLEL_TRANSFER);
    descr->flags.insert(SHARED);
    descr->flags.insert(PUT);
    return true;
}

void IMB_p_iwrite_shared(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_write_shared(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_p_iwrite_shared, P_IWrite_Shared)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(PARALLEL_TRANSFER);
    descr->flags.insert(SHARED);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_2);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(PUT);
    return true;
}

void IMB_p_write_priv(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_write_indv(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_p_write_priv, P_Write_Priv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(PARALLEL_TRANSFER);
    descr->flags.insert(PRIVATE);
    descr->flags.insert(PUT);
    return true;
}

void IMB_p_iwrite_priv(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_write_indv(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_p_iwrite_priv, P_IWrite_Priv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(PARALLEL_TRANSFER);
    descr->flags.insert(PRIVATE);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_2);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(PUT);
    return true;
}

void IMB_p_write_expl(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_write_expl(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_p_write_expl, P_Write_Expl)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(PARALLEL_TRANSFER);
    descr->flags.insert(EXPLICIT);
    descr->flags.insert(PUT);
    return true;
}

void IMB_p_iwrite_expl(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_write_expl(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_p_iwrite_expl, P_IWrite_Expl)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(PARALLEL_TRANSFER);
    descr->flags.insert(EXPLICIT);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_2);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(PUT);
    return true;
}

void IMB_c_write_indv(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_write_indv(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_c_write_indv, C_Write_Indv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(INDV_BLOCK);
    descr->flags.insert(PUT);
    return true;
}

void IMB_c_iwrite_indv(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_write_indv(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_c_iwrite_indv, C_IWrite_Indv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(INDV_BLOCK);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_2);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(PUT);
    return true;
}

void IMB_c_write_shared(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_write_shared(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_c_write_shared, C_Write_Shared)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(SHARED);
    descr->flags.insert(PUT);
    return true;
}

void IMB_c_iwrite_shared(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_write_shared(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_c_iwrite_shared, C_IWrite_Shared)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(SHARED);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_2);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(PUT);
    return true;
}

void IMB_c_write_expl(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_write_expl(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_c_write_expl, C_Write_Expl)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(EXPLICIT);
    descr->flags.insert(PUT);
    return true;
}

void IMB_c_iwrite_expl(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_write_expl(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_c_iwrite_expl, C_IWrite_Expl)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(EXPLICIT);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_2);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(PUT);
    return true;
}

BENCHMARK(IMB_read_indv, S_Read_Indv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(PRIVATE);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(GET);
    return true;
}

void IMB_s_iread_indv(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_read_indv(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_s_iread_indv, S_IRead_Indv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_2);
    descr->flags.insert(PRIVATE);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(GET);
    return true;
}

BENCHMARK(IMB_read_expl, S_Read_Expl)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(PRIVATE);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(GET);
    return true;
}

void IMB_s_iread_expl(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_read_expl(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_s_iread_expl, S_IRead_Expl)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(SINGLE_TRANSFER);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_2);
    descr->flags.insert(PRIVATE);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(GET);
    return true;
}

void IMB_p_read_indv(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_read_indv(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_p_read_indv, P_Read_Indv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(PARALLEL_TRANSFER);
    descr->flags.insert(INDV_BLOCK);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(GET);
    return true;
}

void IMB_p_iread_indv(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_read_indv(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_p_iread_indv, P_IRead_Indv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(PARALLEL_TRANSFER);
    descr->flags.insert(INDV_BLOCK);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_2);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(GET);
    return true;
}

void IMB_p_read_shared(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_read_shared(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_p_read_shared, P_Read_Shared)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(PARALLEL_TRANSFER);
    descr->flags.insert(SHARED);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(GET);
    return true;
}

void IMB_p_iread_shared(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_read_shared(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_p_iread_shared, P_IRead_Shared)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(PARALLEL_TRANSFER);
    descr->flags.insert(SHARED);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_2);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(GET);
    return true;
}

void IMB_p_read_priv(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_read_indv(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_p_read_priv, P_Read_Priv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(PARALLEL_TRANSFER);
    descr->flags.insert(PRIVATE);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(GET);
    return true;
}

void IMB_p_iread_priv(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_read_indv(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_p_iread_priv, P_IRead_Priv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(PARALLEL_TRANSFER);
    descr->flags.insert(PRIVATE);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_2);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(GET);
    return true;
}

void IMB_p_read_expl(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_read_expl(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_p_read_expl, P_Read_Expl)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(PARALLEL_TRANSFER);
    descr->flags.insert(EXPLICIT);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(GET);
    return true;
}

void IMB_p_iread_expl(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_read_expl(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_p_iread_expl, P_IRead_Expl)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(PARALLEL_TRANSFER);
    descr->flags.insert(EXPLICIT);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_2);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(GET);
    return true;
}

void IMB_c_read_indv(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_read_indv(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_c_read_indv, C_Read_Indv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(INDV_BLOCK);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(GET);
    return true;
}

void IMB_c_iread_indv(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_read_indv(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_c_iread_indv, C_IRead_Indv)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(INDV_BLOCK);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_2);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(GET);
    return true;
}

void IMB_c_read_shared(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_read_shared(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_c_read_shared, C_Read_Shared)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(SHARED);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(GET);
    return true;
}

void IMB_c_iread_shared(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_read_shared(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_c_iread_shared, C_IRead_Shared)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(SHARED);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_2);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(GET);
    return true;
}


void IMB_c_read_expl(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_read_expl(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_c_read_expl, C_Read_Expl)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(EXPLICIT);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(GET);
    return true;
}

void IMB_c_iread_expl(struct comm_info* c_info, int size,
                      struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time) {
    IMB_read_expl(c_info, size, ITERATIONS, RUN_MODE, time);
}

BENCHMARK(IMB_c_iread_expl, C_IRead_Expl)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_I);
    descr->flags.insert(RECVBUF_SIZE_I);
    descr->flags.insert(COLLECTIVE);
    descr->flags.insert(EXPLICIT);
    descr->flags.insert(NONBLOCKING);
    descr->flags.insert(NTIMES_2);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(GET);
    return true;
}

BENCHMARK(IMB_open_close, Open_Close)
{
    descr->flags.insert(DEFAULT);
    descr->flags.insert(SENDBUF_SIZE_0);
    descr->flags.insert(RECVBUF_SIZE_0);
    descr->flags.insert(SYNC);
    descr->flags.insert(NON_AGGREGATE);
    descr->flags.insert(EXPLICIT);
    descr->flags.insert(N_MODES_1);
    descr->flags.insert(NO);
    return true;
}
