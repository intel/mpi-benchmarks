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

CPPFLAGS = $(CPPFLAGS) -DRMA
CFLAGS = $(CFLAGS) -DRMA
C_SRC_DIR = ../$(C_SRC_DIR)
RC_FILE = ../WINDOWS/IMB-RMA_VS_2017/IMB-RMA.rc
RC = rc.exe

C_OBJ = IMB_declare.obj \
             IMB_utils.obj \
             IMB_init.obj \
             IMB_mem_manager.obj \
             IMB_benchlist.obj \
             IMB_parse_name_rma.obj \
             IMB_strgs.obj \
             IMB_err_handler.obj \
             IMB_g_info.obj \
             IMB_warm_up.obj \
             IMB_output.obj \
             IMB_init_transfer.obj \
             IMB_user_set_info.obj \
             IMB_chk_diff.obj \
             IMB_rma_put.obj \
             IMB_cpu_exploit.obj \
             IMB_rma_get.obj \
             IMB_rma_atomic.obj \
             IMB-RMA.res

BECHMARK_SUITE_OBJ = RMA_suite.obj \
                     RMA_benchmark.obj\
                     imb.obj args_parser.obj \
                     args_parser_utests.obj \
                     scope.obj \
                     benchmark_suites_collection.obj

{$(C_SRC_DIR)/}.c.obj:
	$(CC) /I"$(MPI_INCLUDE)" $(CFLAGS) -c $(C_SRC_DIR)/$*.c

{../}.cpp.obj:
	$(CPP) /I"$(MPI_INCLUDE)" /I. $(CPPFLAGS) -c ../$*.cpp

{RMA/}.cpp.obj:
	$(CPP) /I"$(MPI_INCLUDE)" /I. $(CPPFLAGS) -c RMA/$*.cpp

all: resources

resources:
	$(RC) /R /FO RMA/IMB-RMA.res $(RC_FILE)
