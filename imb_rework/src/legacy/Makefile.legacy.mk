CPPFLAGS += -DMPI1
CPPFLAGS += -Ilegacy -Ilegacy/imb/src

BECHMARK_SUITE_SRC += legacy/legacy_MPI1_suite.cpp legacy/legacy_MPI1_benchmark.cpp
LEGACY_SRC = legacy/imb/src/IMB_allgather.c \
legacy/imb/src/IMB_allgatherv.c \
legacy/imb/src/IMB_allreduce.c \
legacy/imb/src/IMB_alltoall.c \
legacy/imb/src/IMB_alltoallv.c \
legacy/imb/src/IMB_bandwidth.c \
legacy/imb/src/IMB_barrier.c \
legacy/imb/src/IMB_bcast.c \
legacy/imb/src/IMB_benchlist.c \
legacy/imb/src/IMB_chk_diff.c \
legacy/imb/src/IMB_cpu_exploit.c \
legacy/imb/src/IMB_declare.c \
legacy/imb/src/IMB_err_handler.c \
legacy/imb/src/IMB_exchange.c \
legacy/imb/src/IMB_gather.c \
legacy/imb/src/IMB_gatherv.c \
legacy/imb/src/IMB_g_info.c \
legacy/imb/src/IMB_init.c \
legacy/imb/src/IMB_init_transfer.c \
legacy/imb/src/IMB_mem_manager.c \
legacy/imb/src/IMB_output.c \
legacy/imb/src/IMB_parse_name_mpi1.c \
legacy/imb/src/IMB_pingping.c \
legacy/imb/src/IMB_pingpong.c \
legacy/imb/src/IMB_reduce.c \
legacy/imb/src/IMB_reduce_scatter.c \
legacy/imb/src/IMB_scatter.c \
legacy/imb/src/IMB_scatterv.c \
legacy/imb/src/IMB_sendrecv.c \
legacy/imb/src/IMB_strgs.c \
legacy/imb/src/IMB_utils.c \
legacy/imb/src/IMB_warm_up.c
LEGACY_OBJ=$(LEGACY_SRC:.c=.o)
ADDITIONAL_OBJ += $(LEGACY_OBJ)
HEADERS += legacy/legacy_benchmark.h \
legacy/reworked_IMB_functions.h

$(BECHMARK_SUITE_SRC): test_header_presence
$(LEGACY_SRC): test_header_presence

test_header_presence:
	@test -f legacy/imb/src/IMB_benchmark.h || ( echo "ERROR: legacy/imb must be a symlink to leagcy IMB source code tree" && false )
