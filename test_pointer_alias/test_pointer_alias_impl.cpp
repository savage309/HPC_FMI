//
//  pointer_aliash.cpp
//  GPAPI
//
//  Created by savage309 on 6.05.15.


#include <stdio.h>
#include "test_pointer_alias.h"
#include "common.h"

namespace PointerAlias {    
    void pointerAliasSameType(float* a, float* b, float* res, size_t size) {
        DISABLE_SIMD
        for (size_t i = 1; i < size - 1; ++i) {
            a[i] += res[i];
            b[i] += res[i];
            
            a[i - 1] *= res[i - 1];
            b[i - 1] *= res[i - 1];
            
            a[i + 1] += res[i + 1];
            b[i + 1] += res[i + 1];
        }
    }
    
    void pointerAliasSameTypeRestrict( float* RESTRICT a,  float*  RESTRICT b,  float* RESTRICT res, size_t size) {
        DISABLE_SIMD
        for (size_t i = 1; i < size - 1; ++i) {
            a[i] += res[i];
            b[i] += res[i];
            
            a[i - 1] *= res[i - 1];
            b[i - 1] *= res[i - 1];
            
            a[i + 1] += res[i + 1];
            b[i + 1] += res[i + 1];
        }
    }
    
    void pointerAliasDifferentType(float* a, A* b, B* res, size_t size) {
        DISABLE_SIMD
        for (size_t i = 1; i < size - 1; ++i) {
            a[i] += ((float*) res)[i];
            ((float*)b)[i] += ((float*) res)[i];
            
            a[i] += ((float*) res)[i];
            ((float*)b)[i] += ((float*)res)[i];
            
            a[i - 1] *= ((float*)res)[i - 1];
            ((float*)b)[i - 1] *= ((float*)res)[i - 1];
            
            a[i + 1] += ((float*)res)[i + 1];
            ((float*)b)[i + 1] += ((float*)res)[i + 1];
        }
    }
    
    
    void pointerAliasDifferentTypeNoCast(float* a, A* b, B* res, size_t size) {
        float* bFloat = (float*)b;
        float* resFloat = (float*)res;
        DISABLE_SIMD
        for (size_t i = 1; i < size - 1; ++i) {
            a[i] += resFloat[i];
            bFloat[i] += resFloat[i];
            
            a[i - 1] *= resFloat[i - 1];
            bFloat[i - 1] *= resFloat[i - 1];
            
            a[i + 1] += resFloat[i + 1];
            bFloat[i + 1] += resFloat[i + 1];
        }
    }
    
       
} //namespace PointerAlias
