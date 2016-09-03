// ======================================================================== //
// Copyright 2009-2015 Intel Corporation. Edited By Blagovest Taskov (taskov@gmail.com)                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "math/math.h"

/* include SSE emulation for Xeon Phi */
#if defined (__MIC__)
//#  include "simd/sse_mic.h"
#  include "simd/mic.h"
#endif

/* include SSE wrapper classes */
#if defined(__SSE__)
#  include "simd/sse.h"
#endif

/* include AVX wrapper classes */
#if defined(__AVX__)
#include "simd/avx.h"

#if QUIRK_001
// glibc/compiler bug: memory allocated via operator new may not be suitably aligned for the requested type, e.g. 32B alignment for embree::avxf

#include <stdlib.h>

	class embree_avxf : public embree::avxf {
	public:
		embree_avxf() {}

		embree_avxf(const embree::avxf& src)
		: embree::avxf(src) {
		}

		static void* operator new(size_t size) noexcept {
			void *ptr;
			if (0 == posix_memalign(&ptr, sizeof(embree_avxf), size))
				return ptr;

			return 0;
		}

		static void* operator new[](size_t size) noexcept {
			void *ptr;
			if (0 == posix_memalign(&ptr, sizeof(embree_avxf), size))
				return ptr;

			return 0;
		}
	};

#else
	typedef embree::avxf embree_avxf;

#endif
#endif

#if defined (__AVX__)
#define AVX_ZERO_UPPER() _mm256_zeroupper()
#else
#define AVX_ZERO_UPPER()
#endif
