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

ifeq ($(origin CC),default)
CC=mpicc
endif

BINARY=IMB-$(TARGET)
BUILDDIR=build_$(TARGET)

ifeq "$(TARGET)" "MPI1"
SRC = IMB.c IMB_utils.c IMB_declare.c  IMB_init.c IMB_mem_manager.c IMB_parse_name_mpi1.c  IMB_benchlist.c IMB_strgs.c \
IMB_err_handler.c IMB_g_info.c  IMB_warm_up.c IMB_output.c \
IMB_pingpong.c IMB_pingping.c IMB_allreduce.c IMB_reduce_scatter.c IMB_reduce.c IMB_exchange.c IMB_bcast.c IMB_barrier.c IMB_allgather.c IMB_allgatherv.c IMB_gather.c IMB_gatherv.c IMB_scatter.c IMB_scatterv.c IMB_alltoall.c IMB_alltoallv.c IMB_sendrecv.c IMB_init_transfer.c IMB_chk_diff.c IMB_cpu_exploit.c IMB_bandwidth.c
endif 
ifeq "$(TARGET)" "EXT"
SRC = IMB.c IMB_utils.c IMB_declare.c  IMB_init.c  IMB_mem_manager.c IMB_benchlist.c IMB_parse_name_ext.c IMB_strgs.c \
IMB_err_handler.c IMB_g_info.c  IMB_warm_up.c IMB_output.c \
IMB_window.c IMB_ones_unidir.c IMB_ones_bidir.c IMB_ones_accu.c IMB_init_transfer.c IMB_user_set_info.c IMB_chk_diff.c
endif
ifeq "$(TARGET)" "IO"
SRC = IMB.c IMB_utils.c IMB_declare.c  IMB_init.c  IMB_mem_manager.c IMB_init_file.c IMB_user_set_info.c\
IMB_benchlist.c IMB_parse_name_io.c IMB_strgs.c \
IMB_err_handler.c IMB_g_info.c  IMB_warm_up.c IMB_output.c IMB_cpu_exploit.c\
IMB_open_close.c IMB_write.c IMB_read.c IMB_init_transfer.c IMB_chk_diff.c
override CFLAGS += -DMPIIO
endif
ifeq "$(TARGET)" "NBC"
SRC = IMB.c IMB_utils.c IMB_declare.c  IMB_init.c IMB_mem_manager.c IMB_parse_name_nbc.c  IMB_benchlist.c IMB_strgs.c \
IMB_err_handler.c IMB_g_info.c  IMB_warm_up.c IMB_output.c \
IMB_allreduce.c IMB_reduce_scatter.c IMB_reduce.c IMB_bcast.c IMB_barrier.c IMB_allgather.c IMB_allgatherv.c IMB_gather.c IMB_gatherv.c IMB_scatter.c IMB_scatterv.c IMB_alltoall.c IMB_alltoallv.c IMB_sendrecv.c IMB_init_transfer.c IMB_chk_diff.c IMB_cpu_exploit.c
endif
ifeq "$(TARGET)" "RMA"
SRC = IMB.c IMB_utils.c IMB_declare.c  IMB_init.c  IMB_mem_manager.c IMB_benchlist.c IMB_parse_name_rma.c IMB_strgs.c \
IMB_err_handler.c IMB_g_info.c  IMB_warm_up.c IMB_output.c  IMB_init_transfer.c IMB_user_set_info.c IMB_chk_diff.c \
IMB_rma_put.c IMB_cpu_exploit.c IMB_rma_get.c IMB_rma_atomic.c
endif

ifeq "$(TARGET)" ""
all:
	make -f Makefile TARGET=MPI1
	make -f Makefile TARGET=NBC 
	make -f Makefile TARGET=RMA 
	make -f Makefile TARGET=EXT 
	make -f Makefile TARGET=IO 
	
clean:
	make -f Makefile TARGET=MPI1 clean
	make -f Makefile TARGET=NBC clean
	make -f Makefile TARGET=RMA clean
	make -f Makefile TARGET=EXT clean
	make -f Makefile TARGET=IO clean
else

OBJ=$(SRC:%.c=$(BUILDDIR)/%.o)

all: $(BUILDDIR) $(BINARY)

$(BUILDDIR):
	-mkdir -p $@

$(BINARY): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

$(BUILDDIR)/%.o: %.c
	$(CC) $(CFLAGS) -D$(TARGET) -DIMB2018 -c $< -o $@

clean:
	rm -rf $(BUILDDIR) $(OBJ) $(BINARY)


# DEPENDENCIES
IMB_declare.h:  IMB_settings.h IMB_comm_info.h IMB_settings_io.h IMB_bnames_mpi1.h \
                IMB_bnames_ext.h IMB_bnames_io.h IMB_err_check.h IMB_appl_errors.h IMB_mem_info.h
	touch IMB_declare.h

IMB.o:              IMB_declare.h IMB_benchmark.h
IMB_init.o:         IMB_declare.h IMB_benchmark.h
IMB_mem_manager.o:          IMB_declare.h IMB_benchmark.h
IMB_benchlist.o:        IMB_declare.h IMB_benchmark.h
IMB_output.o:       IMB_declare.h IMB_benchmark.h
IMB_warm_up.o:          IMB_declare.h IMB_benchmark.h
IMB_chk_diff.o:         IMB_declare.h IMB_chk_diff.c
IMB_declare.o:      IMB_declare.h
IMB_g_info.o:       IMB_declare.h IMB_benchmark.h
IMB_err_handler.o:          IMB_declare.h IMB_appl_errors.h
IMB_init_transfer.o:    IMB_declare.h IMB_benchmark.h IMB_comments.h

# IMB-MPI1:
IMB_parse_name_mpi1.o:  IMB_declare.h IMB_benchmark.h IMB_comments.h
IMB_pingping.o:     IMB_declare.h
IMB_pingpong.o:     IMB_declare.h
IMB_sendrecv.o:     IMB_declare.h
IMB_exchange.o:     IMB_declare.h
IMB_reduce.o:       IMB_declare.h
IMB_reduce_scatter.o:   IMB_declare.h
IMB_allreduce.o:            IMB_declare.h
IMB_bcast.o:        IMB_declare.h
IMB_allgather.o:        IMB_declare.h
IMB_allgatherv.o:       IMB_declare.h
IMB_alltoall.o:        IMB_declare.h
IMB_alltoallv.o:       IMB_declare.h
IMB_barrier.o:      IMB_declare.h
IMB_bandwidth.o:    IMB_declare.h

# IMB-NBC:
IMB_parse_name_nbc.o:  IMB_declare.h IMB_benchmark.h IMB_comments.h

# IMB-EXT:
IMB_parse_name_ext.o:   IMB_declare.h IMB_benchmark.h IMB_comments.h
IMB_window.o:           IMB_declare.h
IMB_ones_unidir.o:      IMB_declare.h
IMB_ones_bidir.o:       IMB_declare.h
IMB_ones_accu.o:        IMB_declare.h

# IMB-IO:
IMB_parse_name_io.o:     IMB_declare.h IMB_benchmark.h IMB_comments.h
IMB_init_file.o:         IMB_declare.h IMB_benchmark.h IMB_comments.h
IMB_open_close.o:        IMB_declare.h IMB_benchmark.h
IMB_write.o:             IMB_declare.h IMB_benchmark.h
IMB_read.o:              IMB_declare.h IMB_benchmark.h

#IMB-RMA:
IMB_parse_name_rma.o:     IMB_declare.h IMB_benchmark.h IMB_comments.h
IMB_rma_put.o:            IMB_declare.h IMB_benchmark.h
IMB_rma_get.o:            IMB_declare.h IMB_benchmark.h
IMB_rma_atomic.o:         IMB_declare.h IMB_benchmark.h

endif
