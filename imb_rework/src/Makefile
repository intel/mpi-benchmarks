#CFLAGS=-g -O0 -Wall -Wextra -pedantic
CPP=mpiicpc
CFLAGS = -DMPI1 -g -Wall -Wextra -pedantic -I. -Ioriginal_imb/src

CFLAGS += -Iosu/mpi/pt2pt/

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

IMB_SRC = imb.cpp args_parser.cpp
IMB_OBJ = $(IMB_SRC:.cpp=.o)

BECHMARK_SUITE_SRC = benchmark_suite.cpp orig_benchmark_MPI1.cpp osu_benchmark_pt2pt.cpp
BECHMARK_SUITE_OBJ = $(BECHMARK_SUITE_SRC:.cpp=.o)

HEADERS = args_parser.h benchmark_suite.h benchmark_suite_MPI1.h benchmark_suites_list.h orig_benchmark.h

all: libyaml-cpp.a imb

libyaml-cpp.a: $(YAMLCPP_OBJ)
	ar qc $@ $^
	ranlib $@

yamltest: $(YAMLCPP_TEST_OBJ) libyaml-cpp.a
	$(CPP) $(CFLAGS) -o $@ $^

imb: $(IMB_OBJ) $(BECHMARK_SUITE_OBJ) libyaml-cpp.a
	$(CPP) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CPP) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(IMB_OBJ) $(YAMLCPP_OBJ) $(BECHMARK_SUITE_OBJ) imb yamltest libyaml-cpp.a

imb.cpp: $(HEADERS)
args_parser.cpp: $(HEADERS)
