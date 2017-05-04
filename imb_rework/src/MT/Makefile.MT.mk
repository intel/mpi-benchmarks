CPPFLAGS += -DMT
CPPFLAGS += -IMT
CXXFLAGS += -fopenmp
#CXXFLAGS += -qopenmp

BECHMARK_SUITE_SRC += MT/MT_suite.cpp MT/MT_benchmark.cpp

HEADERS += MT/MT_benchmark.h MT/MT_suite.h

