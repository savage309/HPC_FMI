#pragma once

namespace Factoriel {
    
    #include <iostream>
    
    
    inline int factoriel0(int n) {
        if(n==0) return 1;
        else return factoriel0(n-1)*n;
    }
    
    inline int factoriel1(int n) {
        int r = 1;
        //DISABLE_SIMD_UNROLL
        for(int k=n; k>0; --k)
            r *= k;
        
        return r;
    }
    
    
    inline int factoriel2(int n) {
        int r = 1;
        //DISABLE_SIMD_UNROLL
        for(int k=1; k<=n; ++k)
            r *= k;
        
        return r;
    }
    
    constexpr int getTestSize() {
        return 100000;
    }
    
    void test() {
        printf("Testing factoriel ...\n");
        
        auto test1 = [&] {
            DISABLE_SIMD_UNROLL
            for(int k=0; k<100; k++) {
                int r1 = factoriel1(getTestSize());
                escape(&r1);
            }
        };
        
        auto test2 = [&] {
            DISABLE_SIMD_UNROLL
            for(int k=0; k<100; k++) {
                int r2 = factoriel2(getTestSize());
                escape(&r2);
            }
        };
        
        auto test0 = [&] {
            DISABLE_SIMD_UNROLL
            for(int k=0; k<100; k++) {
                int r3 = factoriel0(getTestSize());
                escape(&r3);
            }
        };
        ADD_BENCHMARK("Factoriel \t Recursion", test0);
        ADD_BENCHMARK("Factoriel \t Backward", test1);
        ADD_BENCHMARK("Factoriel \t Forward", test2);
        benchpress::run_benchmarks(benchpress::options());
    }

}