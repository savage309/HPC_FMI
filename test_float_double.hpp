#pragma once

#include "diffclock.h"

namespace FloatDouble {
    size_t constexpr getTestSize() {
        return 1_million;
    }
    
    NO_INLINE
    double testDouble(double f) {
        for (int i = 0; i < getTestSize(); ++i) {
            auto t = cos(f);
            if (pow(t, 3.0) < 0.5) {
                t += 5 * t;
                t -= 2 * sin(t);
            }
            f += t;
            if (f > 0.1) {
                f /= 10.0;
                f += randomFloat();
            }
            f *= 196.0;
        }
        return f;
    }
    
    NO_INLINE
    float testFloat(float f) {
        for (int i = 0; i < getTestSize(); ++i) {
            auto t = cosf(f);
            if (powf(t, 3.0f) < 0.5f) {
                t += 5 * t;
                t -= 2 * sinf(t);
            }
            f += t;
            if (f > 0.1f) {
                f /= 10.0f;
                f += randomFloat();
            }
            f *= 196.0f;
        }
        return f;

    }
    
    void test() {
        std::cout << "Testing float vs double ..." << std::endl;
        
        auto d = 0.0;
        
        auto test0 = [&] {
            d += testDouble(d);
        };
        
        auto f = 0.0f;
        auto test1 = [&] {
            f += testFloat(d);
        };
        
        ADD_BENCHMARK("FloatVSDouble \t Double", test0);
        ADD_BENCHMARK("FloatVSDouble \t Float", test1);
        
        benchpress::run_benchmarks(benchpress::options());
        
    }
}
