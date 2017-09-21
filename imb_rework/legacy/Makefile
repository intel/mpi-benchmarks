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
CFLAGS += -DMPIIO
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
	$(CC) $(CFLAGS) -D$(TARGET) -c $< -o $@

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
