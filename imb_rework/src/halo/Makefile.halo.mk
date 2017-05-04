CPPFLAGS += -DHALO
CPPFLAGS += -Ihalo
CXXFLAGS += -qopenmp

BECHMARK_SUITE_SRC += halo/halo_suite.cpp halo/halo_benchmark.cpp

HEADERS += halo/halo_benchmark.h

