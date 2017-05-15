CPPFLAGS += -DHALO
CPPFLAGS += -Ihalo
CXXFLAGS += -fopenmp

BECHMARK_SUITE_SRC += halo/halo_suite.cpp halo/halo_benchmark.cpp

HEADERS += halo/MT_benchmark.h

