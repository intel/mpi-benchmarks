CFLAGS += -DMT
CFLAGS += -IMT -IMT/mpi-mt
CFLAGS += -qopenmp

BECHMARK_SUITE_SRC += MT/MT_suite.cpp MT/MT_benchmark.cpp

HEADERS += MT/MT_benchmark.h

