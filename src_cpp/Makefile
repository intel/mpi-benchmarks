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

all: announce
	
ifdef WITH_YAML_CPP
all: libyaml-cpp.a
else
all:	
endif

ifndef TARGET
TARGET=example
ANNOUNCE=1
else
ANNOUNCE=0
endif

ifdef TARGET
include $(TARGET)/Makefile.*.mk
BINARY:=IMB-$(TARGET)
else
include */Makefile.*.mk
BINARY:=imb
endif

ifeq ($(origin CC),default)
CC=mpiicc
endif
ifeq ($(origin CXX),default)
CXX=mpiicpc
endif
override CPPFLAGS += -I.
override CFLAGS += -g -O0 -Wall -Wno-long-long
override CXXFLAGS += -g -O0 -Wall -Wextra -pedantic -Wno-long-long
ifeq "$(CC)" "icx"
CFLAGS += -std=c++11
endif
ifdef WITH_OPENMP
override CFLAGS += -fopenmp
override CXXFLAGS += -fopenmp
endif

#ifeq ($(origin CC),default)
#CXX=mpicc
#endif
#ifeq ($(origin CXX),default)
#CC=mpicxx
#endif
#override CPPFLAGS += -I.
#override CFLAGS = -g -Wall -Wextra -pedantic -Wno-long-long
#CXXFLAGS = -g -Wall -Wextra -pedantic -Wno-long-long
#ifdef WITH_OPENMP
#override CFLAGS += -qopenmp
#override CXXFLAGS += -qopenmp
#endif



all: $(BINARY)

ifdef WITH_YAML_CPP
override CPPFLAGS += -Iyamlcpp/include -Iyamlcpp/test
YAMLCPP_SRC = yamlcpp/src/aliasmanager.cpp \
yamlcpp/src/binary.cpp \
yamlcpp/src/conversion.cpp \
yamlcpp/src/directives.cpp \
yamlcpp/src/emitfromevents.cpp \
yamlcpp/src/emitter.cpp \
yamlcpp/src/emitterstate.cpp \
yamlcpp/src/emitterutils.cpp \
yamlcpp/src/exp.cpp \
yamlcpp/src/iterator.cpp \
yamlcpp/src/node.cpp \
yamlcpp/src/nodebuilder.cpp \
yamlcpp/src/nodeownership.cpp \
yamlcpp/src/null.cpp \
yamlcpp/src/ostream.cpp \
yamlcpp/src/parser.cpp \
yamlcpp/src/regex.cpp \
yamlcpp/src/scanner.cpp \
yamlcpp/src/scanscalar.cpp \
yamlcpp/src/scantag.cpp \
yamlcpp/src/scantoken.cpp \
yamlcpp/src/simplekey.cpp \
yamlcpp/src/singledocparser.cpp \
yamlcpp/src/stream.cpp \
yamlcpp/src/tag.cpp \
yamlcpp/src/contrib/graphbuilder.cpp \
yamlcpp/src/contrib/graphbuilderadapter.cpp


YAMLCPP_TEST_CPP = yamlcpp/test/emittertests.cpp \
yamlcpp/test/main.cpp \
yamlcpp/test/spectests.cpp \
yamlcpp/test/tests.cpp  \
yamlcpp/test/old-api/parsertests.cpp \
yamlcpp/test/old-api/spectests.cpp 

YAMLCPP_OBJ = $(YAMLCPP_SRC:.cpp=.o)
YAMLCPP_TEST_OBJ = $(YAMLCPP_TEST_SRC:.cpp=.o)
endif

IMB_SRC = imb.cpp args_parser.cpp args_parser_utests.cpp scope.cpp
IMB_OBJ = $(IMB_SRC:.cpp=.o)

BECHMARK_SUITE_SRC += benchmark_suites_collection.cpp
BECHMARK_SUITE_OBJ = $(BECHMARK_SUITE_SRC:.cpp=.o)

HEADERS += args_parser.h \
benchmark.h \
benchmark_suite_base.h \
benchmark_suite.h \
benchmark_suites_collection.h \
smart_ptr.h \
utils.h \
scope.h

ifdef WITH_YAML_CPP
libyaml-cpp.a: $(YAMLCPP_OBJ)
	ar qc $@ $^
	ranlib $@

yamltest: $(YAMLCPP_TEST_OBJ) libyaml-cpp.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^

YAML_CPP_LIB = libyaml-cpp.a
YAML_TEST = yamltest
override CPPFLAGS += -DWITH_YAML_CPP
endif

announce:
	@if test "$(ANNOUNCE)" -eq "1"; then echo "NOTE: Building target: $(TARGET), binary name: $(BINARY)"; fi
	@if test "$(ANNOUNCE)" -eq "1"; then echo "NOTE: Use make TARGET=<DIR_NAME> to select a target suite"; fi

$(BINARY): $(IMB_OBJ) $(BECHMARK_SUITE_OBJ) $(ADDITIONAL_OBJ) $(YAML_CPP_LIB)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean: announce
	rm -f $(IMB_OBJ) $(YAMLCPP_OBJ) $(BECHMARK_SUITE_OBJ) $(ADDITIONAL_OBJ) $(BINARY) $(YAML_CPP_LIB) $(YAML_TEST)


$(BECHMARK_SUITE_OBJ): $(HEADERS) Makefile
$(IMB_OBJ): $(HEADERS) Makefile

ifdef WITH_YAML_CPP
$(YAMLCPP_OBJ): Makefile
endif

.PHONY: announce clean all

