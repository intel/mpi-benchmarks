CFLAGS += -DMT
CFLAGS += -IMT
CFLAGS += -qopenmp

BECHMARK_SUITE_SRC += MT/MT_suite.cpp MT/MT_benchmark.cpp

HEADERS += MT/MT_benchmark.h MT/MT_suite.h

