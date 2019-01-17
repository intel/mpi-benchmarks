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

#define BENCHMARK(BMRK_FN, BMRK_NAME) template class OriginalBenchmark<BenchmarkSuite<BS_IO>, BMRK_FN>; \
DECLARE_INHERITED_TEMPLATE(GLUE_TYPENAME(OriginalBenchmark<BenchmarkSuite<BS_IO>, BMRK_FN>), BMRK_NAME) \
template<> smart_ptr<Bmark_descr> OriginalBenchmark<BenchmarkSuite<BS_IO>, BMRK_FN>::descr = NULL; \
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
