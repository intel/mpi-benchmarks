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

all: IMB-MPI1 IMB-NBC IMB-RMA IMB-EXT IMB-IO IMB-MT IMB-P2P

IMB-MPI1:
	make -C src_cpp -f Makefile TARGET=MPI1
	@cp src_cpp/IMB-MPI1 .

IMB-NBC:
	make -C src_cpp -f Makefile TARGET=NBC
	@cp src_cpp/IMB-NBC .

IMB-EXT:
	make -C src_cpp -f Makefile TARGET=EXT
	@cp src_cpp/IMB-EXT .

IMB-RMA:
	make -C src_cpp -f Makefile TARGET=RMA
	@cp src_cpp/IMB-RMA .

IMB-IO:
	make -C src_cpp -f Makefile TARGET=IO
	@cp src_cpp/IMB-IO .

IMB-MT: | IMB-MPI1
	make -C src_cpp -f Makefile TARGET=MT
	@cp src_cpp/IMB-MT .

IMB-P2P:
	make -C src_c/P2P -f Makefile TARGET=P2P
	@cp src_c/P2P/IMB-P2P .

IMB-MPI1-GPU:
	make -C src_cpp -f Makefile TARGET=MPI1 GPU_ENABLE=1
	@cp src_cpp/IMB-MPI1 ./IMB-MPI1-GPU


clean:
	make -C src_cpp -f Makefile TARGET=MPI1 clean
	make -C src_cpp -f Makefile TARGET=NBC clean
	make -C src_cpp -f Makefile TARGET=RMA clean
	make -C src_cpp -f Makefile TARGET=EXT clean
	make -C src_cpp -f Makefile TARGET=IO clean
	make -C src_cpp -f Makefile TARGET=MT clean
	make -C src_c/P2P -f Makefile TARGET=P2P clean
	make -C src_cpp -f Makefile TARGET=MPI1 clean GPU_ENABLE=1
	rm -f IMB-MPI1 IMB-NBC IMB-RMA IMB-EXT IMB-IO IMB-MT IMB-P2P IMB-MPI1-GPU
