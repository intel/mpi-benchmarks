#pragma once

enum benchmark_suite_t { 
#ifdef MPI1    
    BS_MPI1, 
#endif
#ifdef NBC    
    BS_NBC,
#endif
    BS_OSU,
    BS_DUMMY        
};
