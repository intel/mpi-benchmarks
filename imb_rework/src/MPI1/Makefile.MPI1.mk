include legacy/Makefile.*.mk

CPPFLAGS += -DMPI1

BECHMARK_SUITE_SRC += MPI1/legacy_MPI1_suite.cpp MPI1/legacy_MPI1_benchmark.cpp
LEGACY_SRC = $(LEGACY_SRC_DIR)/IMB_allgather.c \
$(LEGACY_SRC_DIR)/IMB_allgatherv.c \
$(LEGACY_SRC_DIR)/IMB_allreduce.c \
$(LEGACY_SRC_DIR)/IMB_alltoall.c \
$(LEGACY_SRC_DIR)/IMB_alltoallv.c \
$(LEGACY_SRC_DIR)/IMB_bandwidth.c \
$(LEGACY_SRC_DIR)/IMB_barrier.c \
$(LEGACY_SRC_DIR)/IMB_bcast.c \
$(LEGACY_SRC_DIR)/IMB_benchlist.c \
$(LEGACY_SRC_DIR)/IMB_chk_diff.c \
$(LEGACY_SRC_DIR)/IMB_cpu_exploit.c \
$(LEGACY_SRC_DIR)/IMB_declare.c \
$(LEGACY_SRC_DIR)/IMB_err_handler.c \
$(LEGACY_SRC_DIR)/IMB_exchange.c \
$(LEGACY_SRC_DIR)/IMB_gather.c \
$(LEGACY_SRC_DIR)/IMB_gatherv.c \
$(LEGACY_SRC_DIR)/IMB_g_info.c \
$(LEGACY_SRC_DIR)/IMB_init.c \
$(LEGACY_SRC_DIR)/IMB_init_transfer.c \
$(LEGACY_SRC_DIR)/IMB_mem_manager.c \
$(LEGACY_SRC_DIR)/IMB_output.c \
$(LEGACY_SRC_DIR)/IMB_parse_name_mpi1.c \
$(LEGACY_SRC_DIR)/IMB_pingping.c \
$(LEGACY_SRC_DIR)/IMB_pingpong.c \
$(LEGACY_SRC_DIR)/IMB_reduce.c \
$(LEGACY_SRC_DIR)/IMB_reduce_scatter.c \
$(LEGACY_SRC_DIR)/IMB_scatter.c \
$(LEGACY_SRC_DIR)/IMB_scatterv.c \
$(LEGACY_SRC_DIR)/IMB_sendrecv.c \
$(LEGACY_SRC_DIR)/IMB_strgs.c \
$(LEGACY_SRC_DIR)/IMB_utils.c \
$(LEGACY_SRC_DIR)/IMB_warm_up.c
LEGACY_OBJ=$(subst $(LEGACY_SRC_DIR),.,$(LEGACY_SRC:.c=.o))
ADDITIONAL_OBJ += $(LEGACY_OBJ)

%.o: $(LEGACY_SRC_DIR)/%.c
	$(CC) -DMPI1 -c -o $@ $<

$(BECHMARK_SUITE_SRC): test_header_presence
$(LEGACY_SRC): test_header_presence

test_header_presence:
	@test -f $(LEGACY_SRC_DIR)/IMB_benchmark.h || ( echo "ERROR: can't find legacy IMB source code tree" && false )
