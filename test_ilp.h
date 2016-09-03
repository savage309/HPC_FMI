#pragma once

#include "diffclock.h"
#include <memory>
#include <algorithm>
#include <limits>
#include <iostream>
#include "common.h"

namespace ILP {    
    constexpr size_t getTestSize() {
        return 1_million;
    }
    
    void testDependency0() {
        float x = randomFloat();
        float y = randomFloat();
        float z = randomFloat();
        float q = 0;
        float a = randomFloat();
        float b = randomFloat();
        
        auto test0 = [&](){
            UNROLL
            for (int i = 0 ; i < getTestSize(); ++i) {
                x = y * z;
                q += z + x + x;
                x = a + b;
            }
        };
        
        
        auto test1 = [&]() {
            UNROLL
            for (int i = 0 ; i < getTestSize(); ++i) {
                const float x0 = y * z;
                q += z + x0 + x0;
                x = a + b;
            }
        };
        
        ADD_BENCHMARK("ILP \t Dependency#0 Dependency", test0);
        ADD_BENCHMARK("ILP \t Dependency#0 No dependecy", test1);
        
        benchpress::run_benchmarks(benchpress::options());
    }
    
    void testDependency1() {
        std::unique_ptr<float[]> a(new float[getTestSize()]);
        std::unique_ptr<float[]> b(new float[getTestSize()]);
        std::for_each(a.get(), a.get() + getTestSize(), [](float) { return randomFloat();});
        std::for_each(b.get(), b.get() + getTestSize(), [](float) { return randomFloat();});
        
        auto test0 = [&] {
            UNROLL
            for (int i = 0; i < getTestSize(); ++i) {
                a[i] = b[i];
                b[i] = b[i] + randomFloat();
            }
        };
        
        auto test1 = [&] {
            UNROLL
            for (int i = 0; i < getTestSize(); ++i) {
                const float temp = b[i];
                a[i] = temp;
                b[i] = temp + randomFloat();
            }
        };

        ADD_BENCHMARK("ILP \t Dependency#1 Dependency", test0);
        ADD_BENCHMARK("ILP \t Dependency#1 No dependecy", test1);
        benchpress::run_benchmarks(benchpress::options());
    }
    
    void testConstants0() {
        float x = randomFloat();
        float y = randomFloat();
        float z = 0.f;
       
        auto test0 = [&]{
            UNROLL
            for (int i = 0; i < getTestSize(); ++i) {
                //sinf might have side effects ...
                z += .5f * sinf(x + y);
                y += z * sinf(x + y) * 42.f;
            }
        };
        //

        auto test1 = [&] {
            const float temp = sinf(x + y);
            UNROLL
            for (int i = 0; i < getTestSize(); ++i) {
                z += .5f * temp;
                y += z * temp * 42.f;
            }
        };
        
        ADD_BENCHMARK("ILP \t Constants InLoop", test0);
        ADD_BENCHMARK("ILP \t Constants BeforeLoop", test1);
        benchpress::run_benchmarks(benchpress::options());
    }
    
    void testLoopBranch0() {
        float x = randomFloat();
        float y = randomFloat();
        float z = 0.f;
        
        auto test0 = [&] {
            UNROLL
            for (int i = 0; i < getTestSize(); ++i) {
                if (x + y < 0.5f) {
                    z += sinf(x + i);
                } else {
                    z += cosf(y + i);
                }
            }
        };

        //

        auto test1 = [&] {
            if (x + y < 0.5f) {
                UNROLL
                for (int i = 0; i < getTestSize(); ++i) {
                    z += sinf(x + i);
                }
            } else {
                UNROLL
                for (int i = 0; i < getTestSize(); ++i) {
                    z += cosf(y + i);
                }
            }
        };
     
        ADD_BENCHMARK("ILP \t LoopWithBranch", test0);
        ADD_BENCHMARK("ILP \t LoopWithOutBranch", test1);
        benchpress::run_benchmarks(benchpress::options());

    }

    void testReduction() {
        std::unique_ptr<int[]> arr(new int[getTestSize()]);
        std::for_each(arr.get(), arr.get() + getTestSize(), [](int){ return randomInt(0, 196); });
        int v = std::numeric_limits<int>::min();
        
        auto test0 = [&] {
            UNROLL
            for (int i = 0; i < getTestSize(); ++i) {
                v = std::max(v, arr[i]);
            }
            
        };

        auto test1 = [&] {
            int& v0 = v;
            int v1 = std::numeric_limits<int>::min();
            int v2 = std::numeric_limits<int>::min();
            int v3 = std::numeric_limits<int>::min();
            UNROLL
            for (int i = 0; i < getTestSize(); i+=4) {
                v0 = std::max(v0, arr[i + 0]);
                v1 = std::max(v1, arr[i + 1]);
                v2 = std::max(v2, arr[i + 2]);
                v3 = std::max(v3, arr[i + 3]);
            }
            v0 = std::max(v0, v1);
            v2 = std::max(v2, v3);
            v0 = std::max(v0, v2);
        };
        
        
        ADD_BENCHMARK("ILP \t Reduction", test0);
        ADD_BENCHMARK("ILP \t Reduction No dependency", test1);
        benchpress::run_benchmarks(benchpress::options());
    }
   
    void test() {
        std::cout << "Testing ILP ..." << std::endl;
        
        testDependency0();
        
        testDependency1();
        
        testConstants0();
        
        testLoopBranch0();
        
        testReduction();
    }
}
