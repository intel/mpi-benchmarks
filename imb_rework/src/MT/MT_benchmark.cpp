#include <vector>
#include <string>
#include <map>
#include <set>
#include <iostream>

#include "MT_benchmark.h" 
#include "MT_suite.h"

using namespace std;

template class PingPongMT<MTBenchmarkSuite>;
DECLARE_INHERITED(PingPongMT<MTBenchmarkSuite>, PingPongMT)

