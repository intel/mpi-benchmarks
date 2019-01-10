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



include helpers/Makefile.*.mk

override CPPFLAGS += -DNBC

BECHMARK_SUITE_SRC += NBC/NBC_suite.cpp NBC/NBC_benchmark.cpp
C_SRC = $(C_SRC_DIR)/IMB_allgather.c \
$(C_SRC_DIR)/IMB_allgatherv.c \
$(C_SRC_DIR)/IMB_allreduce.c \
$(C_SRC_DIR)/IMB_alltoall.c \
$(C_SRC_DIR)/IMB_alltoallv.c \
$(C_SRC_DIR)/IMB_barrier.c \
$(C_SRC_DIR)/IMB_bcast.c \
$(C_SRC_DIR)/IMB_benchlist.c \
$(C_SRC_DIR)/IMB_chk_diff.c \
$(C_SRC_DIR)/IMB_cpu_exploit.c \
$(C_SRC_DIR)/IMB_declare.c \
$(C_SRC_DIR)/IMB_err_handler.c \
$(C_SRC_DIR)/IMB_gather.c \
$(C_SRC_DIR)/IMB_gatherv.c \
$(C_SRC_DIR)/IMB_g_info.c \
$(C_SRC_DIR)/IMB_init.c \
$(C_SRC_DIR)/IMB_init_transfer.c \
$(C_SRC_DIR)/IMB_mem_manager.c \
$(C_SRC_DIR)/IMB_output.c \
$(C_SRC_DIR)/IMB_parse_name_nbc.c \
$(C_SRC_DIR)/IMB_reduce.c \
$(C_SRC_DIR)/IMB_reduce_scatter.c \
$(C_SRC_DIR)/IMB_scatter.c \
$(C_SRC_DIR)/IMB_scatterv.c \
$(C_SRC_DIR)/IMB_sendrecv.c \
$(C_SRC_DIR)/IMB_strgs.c \
$(C_SRC_DIR)/IMB_utils.c \
$(C_SRC_DIR)/IMB_warm_up.c
C_OBJ=$(subst $(C_SRC_DIR),NBC,$(C_SRC:.c=.o))
ADDITIONAL_OBJ += $(C_OBJ)

NBC/%.o: $(C_SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -DNBC -c -o $@ $<
