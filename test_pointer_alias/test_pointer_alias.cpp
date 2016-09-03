#include <stdio.h>
#include "test_pointer_alias.h"
#include "diffclock.h"
#include <iostream>
#include "simd/simd.h"

namespace PointerAlias {
    void test() {
        std::cout << "Testing pointer alias ..." << std::endl;
        auto POINTER_ALIAS_TEST_SIZE = PointerAlias::getTestSize();
        std::unique_ptr<float[]> a(new float[POINTER_ALIAS_TEST_SIZE]);
        std::unique_ptr<float[]> b(new float[POINTER_ALIAS_TEST_SIZE]);
        std::unique_ptr<float[]> res(new float[POINTER_ALIAS_TEST_SIZE]);
        
        PointerAlias::pointerAliasSameType(a.get(), b.get(), res.get(), POINTER_ALIAS_TEST_SIZE);
        
        for (auto i = 0; i < POINTER_ALIAS_TEST_SIZE; ++i) {
            a[i] = i; b[i] = i * 2; res[i] = i * 3;
        }
       
        auto test0 = [&] {
            PointerAlias::A* bInt = (PointerAlias::A*)b.get();
            PointerAlias::B* resUnsigned = (PointerAlias::B*)res.get();
            PointerAlias::pointerAliasDifferentType(a.get(), bInt, resUnsigned, POINTER_ALIAS_TEST_SIZE);
        };
        
        for (auto i = 0; i < POINTER_ALIAS_TEST_SIZE; ++i) {
            a[i] = i; b[i] = i * 2; res[i] = i * 3;
        }
        
        auto test1 = [&] {
            PointerAlias::pointerAliasSameType(a.get(), b.get(), res.get(), POINTER_ALIAS_TEST_SIZE);
        };
        
        for (auto i = 0; i < POINTER_ALIAS_TEST_SIZE; ++i) {
            a[i] = i; b[i] = i * 2; res[i] = i * 3;
        }
        
        auto test2 = [&] {
            PointerAlias::pointerAliasDifferentTypeNoCast(a.get(), (PointerAlias::A*)b.get(), (PointerAlias::B*)res.get(), POINTER_ALIAS_TEST_SIZE);
        };
        
        for (auto i = 0; i < POINTER_ALIAS_TEST_SIZE; ++i) {
            a[i] = i; b[i] = i * 2; res[i] = i * 3;
        }
        
        auto test3 = [&] {
            PointerAlias::pointerAliasSameTypeRestrict(a.get(), b.get(), res.get(), POINTER_ALIAS_TEST_SIZE);
        };
        
        std::unique_ptr<embree::ssef[]> sseA(new embree::ssef[getTestSize()/embree::ssef::size]);
        std::unique_ptr<embree::ssef[]> sseB(new embree::ssef[getTestSize()/embree::ssef::size]);
        std::unique_ptr<embree::ssef[]> sseRes(new embree::ssef[getTestSize()/embree::ssef::size]);
        
        for (int i = 0; i < getTestSize(); i+=embree::ssef::size) {
            sseA[i/embree::ssef::size].load(a.get()+i);
            sseB[i/embree::ssef::size].load(b.get()+i);
            sseRes[i/embree::ssef::size].load(res.get()+i);
        }
        
        for (auto i = 0; i < POINTER_ALIAS_TEST_SIZE; ++i) {
            a[i] = i; b[i] = i * 2; res[i] = i * 3;
        }
        
        auto test4 = [&] {
            PointerAlias::pointerSIMD<embree::ssef>(sseA.get(), sseB.get(), sseRes.get(), POINTER_ALIAS_TEST_SIZE/embree::ssef::size);
        };
        
        for (auto i = 0; i < POINTER_ALIAS_TEST_SIZE; ++i) {
            a[i] = i; b[i] = i * 2; res[i] = i * 3;
        }
#if (defined  __AVX__)
        std::unique_ptr<embree_avxf[]> avxA(new embree_avxf[getTestSize()/embree::avxf::size]);
        std::unique_ptr<embree_avxf[]> avxB(new embree_avxf[getTestSize()/embree::avxf::size]);
        std::unique_ptr<embree_avxf[]> avxRes(new embree_avxf[getTestSize()/embree::avxf::size]);
        
        for (int i = 0; i < getTestSize(); i+=embree::avxf::size) {
            avxA[i/embree::avxf::size].load(a.get()+i);
            avxB[i/embree::avxf::size].load(b.get()+i);
            avxRes[i/embree::avxf::size].load(res.get()+i);
        }

        auto test5 = [&] {
            PointerAlias::pointerSIMD<embree::avxf>(avxA.get(), avxB.get(), avxRes.get(), POINTER_ALIAS_TEST_SIZE/embree::avxf::size);
        };

#endif//__AVX__
        
        ADD_BENCHMARK("PointerAlias \t DifferentType", test0);
        ADD_BENCHMARK("PointerAlias \t SameType", test1);
        ADD_BENCHMARK("PointerAlias \t DifferentTypeNoCast", test2);
        ADD_BENCHMARK("PointerAlias \t SameTypeRestrict", test3);
        ADD_BENCHMARK("PointerAlias \t SSE", test4);
#if (defined __AVX__)
        ADD_BENCHMARK("PointerAlias \t AVX", test5);
#endif
        benchpress::run_benchmarks(benchpress::options());
    }
    
} //namespace PointerAlias
