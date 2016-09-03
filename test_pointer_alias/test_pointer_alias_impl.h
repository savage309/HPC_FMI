#pragma once

#include "common.h"
#include "simd/simd.h"

namespace PointerAlias {
    struct A;
    struct B;
    void pointerAliasSameType(float* a, float* b, float* res, size_t size);
    void pointerAliasDifferentType(float* a, A* b, B* res, size_t size);
    void pointerAliasDifferentTypeNoCast(float* a, A* b, B* res, size_t size) ;
    void pointerAliasSameTypeRestrict( float* RESTRICT a,  float*  RESTRICT b,  float* RESTRICT res, size_t size);
    template <typename SIMD>
    void pointerSIMD(SIMD* RESTRICT sseA, SIMD* RESTRICT sseB, SIMD* RESTRICT sseRes, size_t size) {
        using namespace embree;
        for (int i = 1; i < size - 1; ++i) {
            sseA[i] += sseRes[i];
            sseB[i] += sseRes[i];
            
            sseA[i - 1] *= sseRes[i - 1];
            sseB[i - 1] *= sseRes[i - 1];
            
            sseA[i + 1] += sseRes[i + 1];
            sseB[i + 1] += sseRes[i + 1];
        }
    }


} //namespace PointerAlias

