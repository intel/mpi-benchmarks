#*****************************************************************************
# *                                                                           *
# * Copyright Intel Corporation.                                              *
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

