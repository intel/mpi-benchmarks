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

TARGET=P2P
BINARY:=IMB-P2P

override CFLAGS += -O3 -Wall -Wextra -pedantic -Wno-long-long
override LDFLAGS += -lm

ifeq ($(origin CC),default)
CC=mpiicc
endif

all: $(BINARY)

IMB_SRC = imb_p2p.c
IMB_SRC += imb_p2p_pingpong.c
IMB_SRC += imb_p2p_pingping.c
IMB_SRC += imb_p2p_unirandom.c
IMB_SRC += imb_p2p_birandom.c
IMB_SRC += imb_p2p_corandom.c
IMB_SRC += imb_p2p_stencil2d.c
IMB_SRC += imb_p2p_stencil3d.c
IMB_SRC += imb_p2p_sendrecv_replace.c

IMB_OBJ = $(IMB_SRC:.c=.o)

$(BINARY): $(IMB_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(IMB_OBJ) $(BINARY)

$(IMB_OBJ): $(HEADERS) Makefile

.PHONY: clean all
