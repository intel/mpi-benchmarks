# *****************************************************************************
# *                                                                           *
# * Copyright (C) 2023 Intel Corporation                                      *
# *                                                                           *
# *****************************************************************************

# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:

# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
# OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#  ***************************************************************************

!INCLUDE  helpers/Makefile_win.helpers.mk

CPPFLAGS = $(CPPFLAGS) -DNBC
CFLAGS = $(CFLAGS) -DNBC
C_SRC_DIR = ../$(C_SRC_DIR)
RC_FILE = ../WINDOWS/IMB-NBC_VS_2017/IMB-NBC.rc
RC = rc.exe

C_OBJ = IMB_allgather.obj \
             IMB_allgatherv.obj \
             IMB_allreduce.obj \
             IMB_alltoall.obj \
             IMB_alltoallv.obj \
             IMB_barrier.obj \
             IMB_bcast.obj \
             IMB_benchlist.obj \
             IMB_chk_diff.obj \
             IMB_cpu_exploit.obj \
             IMB_declare.obj \
             IMB_err_handler.obj \
             IMB_gather.obj \
             IMB_gatherv.obj \
             IMB_g_info.obj \
             IMB_init.obj \
             IMB_init_transfer.obj \
             IMB_mem_manager.obj \
             IMB_output.obj \
             IMB_parse_name_nbc.obj \
             IMB_reduce.obj \
             IMB_reduce_scatter.obj \
             IMB_scatter.obj \
             IMB_scatterv.obj \
             IMB_sendrecv.obj \
             IMB_strgs.obj \
             IMB_utils.obj \
             IMB_warm_up.obj \
             IMB-NBC.res

BECHMARK_SUITE_OBJ = NBC_suite.obj \
                     NBC_benchmark.obj\
                     imb.obj args_parser.obj \
                     args_parser_utests.obj \
                     scope.obj \
                     benchmark_suites_collection.obj

{$(C_SRC_DIR)/}.c.obj:
	$(CC) /I"$(MPI_INCLUDE)" $(CFLAGS) -c $(C_SRC_DIR)/$*.c

{../}.cpp.obj:
	$(CPP) /I"$(MPI_INCLUDE)" /I. $(CPPFLAGS) -c ../$*.cpp

{NBC/}.cpp.obj:
	$(CPP) /I"$(MPI_INCLUDE)" /I. $(CPPFLAGS) -c NBC/$*.cpp

all: resources

resources:
	$(RC) /R /FO NBC/IMB-NBC.res $(RC_FILE)
