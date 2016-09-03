#pragma once

#include "common.h"
#include <cmath>
#include <iostream>
#include "test_inline_impl.h"

namespace Inline {
    INLINE float calcInline(float f) {
        if (f > 0)
            return .5f;
        else return cos(sin(atan(f)));
    }

    constexpr size_t getTestSize() {
        return 2_million;
    }
    
    void test(){
        std::cout << "Testing inlined functions ..." << std::endl;
        float res = 0.f;
        const auto TEST_SIZE = getTestSize();

        auto test0 = [&] {
        for(size_t a = 0; a < TEST_SIZE; ++a)
            for(int b = 0; b < 5; ++b)
                res += calcNoInline(b);
        };
        
        auto test1 = [&] {
            for(size_t a = 0; a < TEST_SIZE; ++a)
                for(int b = 0; b < 5; ++b)
                    res += calcInline(b);
        };
        
        ADD_BENCHMARK("Inlining \t NoInline", test0);
        ADD_BENCHMARK("Inlining \t Inline", test1);
        benchpress::run_benchmarks(benchpress::options());
    }

} //namespace Inline
