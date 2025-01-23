# *****************************************************************************
# *                                                                           *
# * Copyright (C) 2024 Intel Corporation                                      *
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

include helpers/Makefile.*.mk

override CPPFLAGS += -DRMA

BECHMARK_SUITE_SRC += RMA/RMA_suite.cpp RMA/RMA_benchmark.cpp
C_SRC = $(C_SRC_DIR)/IMB_utils.c \
$(C_SRC_DIR)/IMB_declare.c \
$(C_SRC_DIR)/IMB_init.c \
$(C_SRC_DIR)/IMB_mem_manager.c \
$(C_SRC_DIR)/IMB_benchlist.c \
$(C_SRC_DIR)/IMB_parse_name_rma.c \
$(C_SRC_DIR)/IMB_strgs.c \
$(C_SRC_DIR)/IMB_err_handler.c \
$(C_SRC_DIR)/IMB_g_info.c \
$(C_SRC_DIR)/IMB_warm_up.c \
$(C_SRC_DIR)/IMB_output.c \
$(C_SRC_DIR)/IMB_init_transfer.c \
$(C_SRC_DIR)/IMB_user_set_info.c \
$(C_SRC_DIR)/IMB_chk_diff.c \
$(C_SRC_DIR)/IMB_rma_put.c \
$(C_SRC_DIR)/IMB_cpu_exploit.c \
$(C_SRC_DIR)/IMB_rma_get.c \
$(C_SRC_DIR)/IMB_rma_atomic.c
ifdef GPU_ENABLE
override C_SRC += $(C_SRC_DIR)/IMB_gpu_common.c
override CPPFLAGS += -DGPU_ENABLE
override LDFLAGS += -ldl
ifdef CUDA_INCLUDE_DIR
override C_SRC += $(C_SRC_DIR)/IMB_cuda.c \
$(C_SRC_DIR)/IMB_cuda_api.c
override CPPFLAGS += -I${CUDA_INCLUDE_DIR} -DCUDA_INCLUDE_DIR
endif
ifdef ZE_INCLUDE_DIR
override C_SRC += $(C_SRC_DIR)/IMB_ze.c \
$(C_SRC_DIR)/IMB_ze_api.c
override CPPFLAGS += -I${ZE_INCLUDE_DIR} -DZE_INCLUDE_DIR
endif
ifndef CUDA_INCLUDE_DIR
ifndef ZE_INCLUDE_DIR
ifneq (clean, $(filter clean,$(MAKECMDGOALS)))
$(error CUDA_INCLUDE_DIR and ZE_INCLUDE_DIR are not set) 
endif
endif
endif
SUBDIR:=GPU
else
SUBDIR:=CPU
endif

C_OBJ=$(subst $(C_SRC_DIR),RMA/$(SUBDIR),$(C_SRC:.c=.o))
ADDITIONAL_OBJ += $(C_OBJ)
BECHMARK_SUITE_LOCAL_OBJ=$(subst RMA/,RMA/$(SUBDIR)/,$(BECHMARK_SUITE_LOCAL_SRC:.cpp=.o))
ADDITIONAL_OBJ += $(BECHMARK_SUITE_LOCAL_OBJ)

RMA/$(SUBDIR)/%.o: $(C_SRC_DIR)/%.c $(SUBDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -DRMA -c -o $@ $<

RMA/$(SUBDIR)/%.o: RMA/%.cpp $(SUBDIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -DRMA -c -o $@ $<

${SUBDIR}:
	mkdir -p RMA/${SUBDIR}
