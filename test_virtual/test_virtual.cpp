
#include "test_virtual.h"
#include "handmade_virtual.h"
#include "native_virtual.h"
#include "diffclock.h"
#include <iostream>

namespace Virtual {
    constexpr size_t getTestSize() {
        return 1_million;
    }
    
    void test() {
        Virtual::init();
        
        std::cout << "Testing handmade virtual functions ...\n";
        
        size_t testSize = getTestSize();
        
        using namespace Virtual;
        
        std::unique_ptr<std::unique_ptr<Base>[]> base(new std::unique_ptr<Base>[testSize]);
        
        for (int i = 0; i < testSize; ++i) {
            if (i % 2) {
                base[i].reset(new Derived);
            } else {
                base[i].reset(new Derived2);
            }
        }
        
        auto test0 = [&] {
            for (int i = 0; i < testSize; ++i) {
                base[i]->set(i * i);
                if (base[i]->get() > 5) {
                    base[i]->set(base[i]->get() + 1);
                }
            }
        };
        
        std::unique_ptr<std::unique_ptr<NBase>[]> nbase(new std::unique_ptr<NBase>[testSize]);
        
    
        for (int i = 0; i < testSize; ++i) {
            if (i % 2) {
                nbase[i].reset(new NDerived);
            } else {
                nbase[i].reset(new NDerived2);
            }
        }
        
        auto test1 = [&] {
            for (int i = 0; i < testSize; ++i) {
                nbase[i]->set(i * i);
                if (nbase[i]->get() > 5) {
                    nbase[i]->set(nbase[i]->get() + 1);
                }
            }
        };
        
        ADD_BENCHMARK("VirtualFunctions \t Native", test0);
        ADD_BENCHMARK("VirtualFunctions \t HandMade", test1);
        
        benchpress::run_benchmarks(benchpress::options());

    }
}