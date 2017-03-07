all: libyaml-cpp.a imb

#CFLAGS=-g -O0 -Wall -Wextra -pedantic
CPP=mpiicpc
CC=mpiicc
CFLAGS = -g -Wall -Wextra -pedantic -I. -DCOMM_EXPERIMENT=0

MPI1=ON
OSU=OFF
EXAMPLE=OFF

ifeq ($(MPI1),ON)
include Makefile.legacy
endif
ifeq ($(OSU),ON)
include Makefile.OSU
endif
ifeq ($(EXAMPLE),ON)
include Makefile.example
endif


CFLAGS += -Iyamlcpp/include -Iyamlcpp/test
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

libyaml-cpp.a: $(YAMLCPP_OBJ)
	ar qc $@ $^
	ranlib $@

yamltest: $(YAMLCPP_TEST_OBJ) libyaml-cpp.a
	$(CPP) $(CFLAGS) -o $@ $^

imb: $(IMB_OBJ) $(BECHMARK_SUITE_OBJ) $(ADDITIONAL_OBJ) libyaml-cpp.a
	$(CPP) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CPP) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(IMB_OBJ) $(YAMLCPP_OBJ) $(BECHMARK_SUITE_OBJ) $(ADDITIONAL_OBJ) imb yamltest libyaml-cpp.a


$(BECHMARK_SUITE_OBJ): $(HEADERS) Makefile
$(IMB_OBJ): $(HEADERS) Makefile
$(YAMLCPP_OBJ): Makefile
