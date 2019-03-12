#*****************************************************************************
# *                                                                           *
# * Copyright 2016-2019 Intel Corporation.                                    *
# *                                                                           *
# *****************************************************************************
#
# This code is covered by the Community Source License (CPL), version
# 1.0 as published by IBM and reproduced in the file "license.txt" in the
# "license" subdirectory. Redistribution in source and binary form, with
# or without modification, is permitted ONLY within the regulations
# contained in above mentioned license.
#
# Use of the name and trademark "Intel(R) MPI Benchmarks" is allowed ONLY
# within the regulations of the "License for Use of "Intel(R) MPI
# Benchmarks" Name and Trademark" as reproduced in the file
# "use-of-trademark-license.txt" in the "license" subdirectory.
#
# THE PROGRAM IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OR
# CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT
# LIMITATION, ANY WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT,
# MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Each Recipient is
# solely responsible for determining the appropriateness of using and
# distributing the Program and assumes all risks associated with its
# exercise of rights under this Agreement, including but not limited to
# the risks and costs of program errors, compliance with applicable
# laws, damage to or loss of data, programs or equipment, and
# unavailability or interruption of operations.
#
# EXCEPT AS EXPRESSLY SET FORTH IN THIS AGREEMENT, NEITHER RECIPIENT NOR
# ANY CONTRIBUTORS SHALL HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING
# WITHOUT LIMITATION LOST PROFITS), HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OR
# DISTRIBUTION OF THE PROGRAM OR THE EXERCISE OF ANY RIGHTS GRANTED
# HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
#
# EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF
# YOUR JURISDICTION. It is licensee's responsibility to comply with any
# export regulations applicable in licensee's jurisdiction. Under
# CURRENT U.S. export regulations this software is eligible for export
# from the U.S. and can be downloaded by or otherwise exported or
# reexported worldwide EXCEPT to U.S. embargoed destinations which
# include Cuba, Iraq, Libya, North Korea, Iran, Syria, Sudan,
# Afghanistan and any other country to which the U.S. has embargoed
# goods and services.
#
#  ***************************************************************************

!INCLUDE  helpers/Makefile_win.helpers.mk

CPPFLAGS = $(CPPFLAGS) -DRMA
CFLAGS = $(CFLAGS) -DRMA
C_SRC_DIR = ../$(C_SRC_DIR)

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
             IMB_rma_atomic.obj

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
