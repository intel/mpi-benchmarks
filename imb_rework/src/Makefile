#CFLAGS=-g -O0 -Wall -Wextra -pedantic
CPP=mpiCC
CC=mpicc
CFLAGS = -DMPI1 -O3 -Wall -Wextra -pedantic -I. -Ioriginal_imb/src -DCOMM_EXPERIMENT=0

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

IMB_SRC = imb.cpp args_parser.cpp args_parser_utests.cpp
IMB_OBJ = $(IMB_SRC:.cpp=.o)

BECHMARK_SUITE_SRC = benchmark_suite.cpp orig_benchmark_MPI1.cpp osu_benchmark_pt2pt.cpp benchmark_suite_MPI1.cpp
LEGACY_SRC = original_imb/src/IMB_allgather.c \
original_imb/src/IMB_allgatherv.c \
original_imb/src/IMB_allreduce.c \
original_imb/src/IMB_alltoall.c \
original_imb/src/IMB_alltoallv.c \
original_imb/src/IMB_bandwidth.c \
original_imb/src/IMB_barrier.c \
original_imb/src/IMB_bcast.c \
original_imb/src/IMB_benchlist.c \
original_imb/src/IMB_chk_diff.c \
original_imb/src/IMB_cpu_exploit.c \
original_imb/src/IMB_declare.c \
original_imb/src/IMB_err_handler.c \
original_imb/src/IMB_exchange.c \
original_imb/src/IMB_gather.c \
original_imb/src/IMB_gatherv.c \
original_imb/src/IMB_g_info.c \
original_imb/src/IMB_init.c \
original_imb/src/IMB_init_transfer.c \
original_imb/src/IMB_mem_manager.c \
original_imb/src/IMB_output.c \
original_imb/src/IMB_parse_name_mpi1.c \
original_imb/src/IMB_pingping.c \
original_imb/src/IMB_pingpong.c \
original_imb/src/IMB_reduce.c \
original_imb/src/IMB_reduce_scatter.c \
original_imb/src/IMB_scatter.c \
original_imb/src/IMB_scatterv.c \
original_imb/src/IMB_sendrecv.c \
original_imb/src/IMB_strgs.c \
original_imb/src/IMB_utils.c \
original_imb/src/IMB_warm_up.c
LEGACY_OBJ=$(LEGACY_SRC:.c=.o)
BECHMARK_SUITE_OBJ = $(BECHMARK_SUITE_SRC:.cpp=.o) $(LEGACY_OBJ)

HEADERS = args_parser.h benchmark_suite.h benchmark_suite_MPI1.h benchmark_suites_list.h orig_benchmark.h smart_ptr.h reworked_IMB_functions.h

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

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(IMB_OBJ) $(YAMLCPP_OBJ) $(BECHMARK_SUITE_OBJ) imb yamltest libyaml-cpp.a

imb.o: $(HEADERS)
args_parser.o: $(HEADERS)
orig_benchmark_MPI1.o: $(HEADERS)
benchmark_suite_MPI1.o: $(HEADERS)
benchmark_suite.o: $(HEADERS)
args_parser_utests.o: $(HEADERS)

