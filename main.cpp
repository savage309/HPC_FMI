#include <iostream>
#include <cmath>
#include <memory>
#include <mutex>
#include <stdlib.h>

#include <algorithm>

#include "common.h"
#include "diffclock.h"

#include "test_cache_miss.hpp"
#include "test_pointer_alias/test_pointer_alias.h"
#include "test_SoA.hpp"
#include "test_inline/test_inline.h"
#include "test_data_oriented_design.hpp"
#include "test_float_double.hpp"
#include "test_virtual/test_virtual.h"
#include "test_ilp.h"
#include "test_threads.h"
#include "test_image.h"
#include "test_factoriel.hpp"

benchpress::registration* benchpress::registration::d_this;

using namespace std;
int main(int argc, const char * argv[]) {

    std::cout << "Starting tests ...\n" << std::endl;
    const auto t0 = getTime();

    /* #0 Instruction level parallelism*/
    ILP::test();
    std::cout << "\n **** \n\n";
    
    ////////////////////////////////////////////////////////////////////
    
    /* #1 Cache misses */
    Image::test();
    std::cout << "\n **** \n\n";
    
    ////////////////////////////////////////////////////////////////////
    
    Virtual::test();
    std::cout << "\n **** \n\n";
    
    ////////////////////////////////////////////////////////////////////
    
    CacheMiss::test();
    std::cout << "\n **** \n\n";
    
    ////////////////////////////////////////////////////////////////////
    
    /* #2 Pointer alliasing */
    PointerAlias::test();
    std::cout << "\n **** \n\n";
    
    ////////////////////////////////////////////////////////////////////

    /* #3 Compiler related */
    Inline::test();
    std::cout << "\n **** \n\n";
    
    ////////////////////////////////////////////////////////////////////

    Factoriel::test();
    std::cout << "\n **** \n\n";
    
    ////////////////////////////////////////////////////////////////////
    
    FloatDouble::test();
    std::cout << "\n **** \n\n";
    
    ////////////////////////////////////////////////////////////////////
    
    /* Data oriented design */
    DataOrientedDesign::test();
    std::cout << "\n **** \n\n";
    
    ////////////////////////////////////////////////////////////////////

    SoA::test();
    std::cout << "\n **** \n\n";
    
    ////////////////////////////////////////////////////////////////////
    
    //Threads::test();
    //std::cout << "\n **** \n\n";
    
    const auto t1 = getTime();
    
    ////////////////////////////////////////////////////////////////////
    
    std::cout << "Tests completed, time " << diffclock(t1, t0) << "s" << std::endl;
    
    return 0;
}

//shared memory & shared nothing
//concurrency & parallelism
//subroutines & couroutines
//test_and_set
//amdahls law
//car-passenger observation
//coffman conditions
//foster methodology
//flynn taxonomy
//conditional variables / monitors
//spin lock, read-write lock, mutex, concurrent collections
//roofline model

