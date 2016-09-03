#pragma once

#include "simd/simd.h"

namespace Image {
    using std::pow;
    embree::ssef exp(const embree::ssef& s) {
        embree::ssef res;
        res.m128 = exp_ps(s.m128);
        return res;
    }
    embree::ssef pow(embree::ssef s, float pow) {
        v4sf logps = (log_ps(s.m128));
        embree::ssef ss;
        ss.m128 = logps;
        return exp_ps(pow*ss);
    }
    
#ifdef __AVX__
    embree::avxf pow(embree::avxf s, float p) {
        //v4sf logps = (log_ps(s.m128));
        //embree::ssef ss;
        //ss.m128 = logps;
        embree::ssef s0 = embree::extract<0>(s);
        embree::ssef s1 = embree::extract<1>(s);
        s0 = pow(s0, p);
        s1 = pow(s1, p);
        return  embree::avxf(s0, s1);
    }
    
    embree::avxf exp2(embree::avxf s, float pow) {
        embree::ssef s0 = embree::extract<0>(s);
        embree::ssef s1 = embree::extract<1>(s);
        s0 = exp(s0);
        s1 = exp(s1);
        return embree::avxf(s0, s1);
    }
#endif
    struct Float {
        enum { size = 1 };
        float f;
        Float() {}
        Float(float f):f(f){}
        Float operator/(Float rhs) {
            return Float(f/rhs.f);
        }
        Float operator/(float rhs) {
            return Float(f/rhs);
        }
        Float& operator*=(float rhs) {
            f *= rhs;
            return *this;
        }
        operator float() const {
            return f;
        }
    };
    
    
    template<typename T>
    T decode(T color) {
        color=pow(color/0.5f, 2.2f)*0.5f;
        color=pow((color+0.055f)/1.055f, 2.4f);
        return color;
    }
    
    template<typename T>
    T correctHDRIColor(T color, int colorSpace, float gamma, float colorMult) {
        color *= colorMult;
        color = pow(color, gamma);
        color = decode(color);
        return color;
    }
    
    template <typename T>
    T apply(int flags, float gammaColor, int colorSpace, T res, T blendColor, float colorMult, float blend) {
        res=correctHDRIColor(res, colorSpace, gammaColor, colorMult);
        res=res*blend + res*((1.0f-blend)*blend);
        return res;
    }
    
    template <typename T>
    T randomColor();
    
    template <>
    Float randomColor<Float>() {
        Float res;
        res.f = randomFloat();
        return res;
    }
    
    template <>
    embree::ssef randomColor<embree::ssef>() {
        embree::ssef f(42.f);
        return f;
    }
#ifdef __AVX__
    template <>
    embree_avxf randomColor<embree_avxf>() {
        embree::avxf f(42.f);
        return f;
    }
#endif

    template <typename T>
    struct Image {
        int width, height, colorSpace, flags;
        float gamma, colorMult;
		std::unique_ptr< T[] > colors;

        Image(int w, int h) {
			width = w;
			height = h / T::size;
			colors.reset(new T[width * height]);
            gamma = randomFloat();
            colorMult = randomFloat();
            colorSpace = randomInt(0, 2);
            flags = randomInt(0, 1);
            for (int i = 0; i < width; ++i) {
                for (int j = 0; j < height; ++j) {
                    colors[i + width * j] = randomColor<T>();
                }
            }
        }
        
    };
  
    float test() {
        printf("Testing image processing...\n");
        const int SIZE = 4096;
        
        Image<Float> img(SIZE, SIZE);
        Image<embree::ssef> img2(SIZE, SIZE);
#ifdef __AVX__
        Image<embree_avxf> img3(SIZE, SIZE);
#endif
        Float blendColor = randomColor<Float>();
        float blend = randomFloat();
        float res = 0.f;
        
        auto test0 = [&] {
            DISABLE_SIMD_UNROLL
            for (int i = 0; i < img.width; ++i) {
                DISABLE_SIMD_UNROLL
                for (int j = 0; j < img.height; ++j) {
                    Float f = apply(img.flags, img.gamma, img.colorSpace, img.colors[i + img.width*j], blendColor, img.colorMult, blend);
                    res += f.f;
                }
            }
        };
        
        
        auto test1 = [&] {
            DISABLE_SIMD_UNROLL
            for (int j = 0; j < img.width; ++j) {
                DISABLE_SIMD_UNROLL
                for (int i = 0; i < img.height; ++i) {
                    Float f = apply(img.flags, img.gamma, img.colorSpace, img.colors[i + img.width*j], blendColor, img.colorMult, blend);
                    res += f.f;
                }
            }
        };
        
        embree::ssef blendColor2(0.f);
        
        embree::ssef resSSE(0.f, 0.f, 0.f, 0.f);
        
        auto test2 = [&] {
            DISABLE_SIMD_UNROLL
            for (int i = 0; i < img2.width; ++i) {
                DISABLE_SIMD_UNROLL
                for (int j = 0; j < img2.height / embree::ssef::size; ++j) {
                    resSSE += apply(img2.flags, img2.gamma, img2.colorSpace, img2.colors[i + img2.width*j], blendColor2, img2.colorMult, blend);
                }
            }
        };
#ifdef __AVX__
        embree::avxf blendColor3(0.f);
        embree::avxf resAVX(0.f);
        
        auto test3 = [&] {
            DISABLE_SIMD_UNROLL
            for (int i = 0; i < img3.width; ++i) {
                DISABLE_SIMD_UNROLL
                for (int j = 0; j < img3.height / embree::avxf::size; ++j) {
                   resAVX += apply(img3.flags, img3.gamma, img3.colorSpace, static_cast< const embree::avxf& >(img3.colors[i + img3.width*j]), blendColor3, img3.colorMult, blend);
                }
            }
        };
#endif

        ADD_BENCHMARK("Image \t Column", test0);
        ADD_BENCHMARK("Image \t Row", test1);
        ADD_BENCHMARK("Image \t SSE", test2);
#ifdef __AVX__
        ADD_BENCHMARK("Image \t AVX", test3);
#endif
        benchpress::run_benchmarks(benchpress::options());
        
        return  res + resSSE[0] + resSSE[1] + resSSE[3] + resSSE[2]
#ifdef __AVX__ 
        + resAVX[0] + resAVX[1] + resAVX[2] + resAVX[3] + resAVX[4] + resAVX[5] + resAVX[6] +resAVX[7]
#endif
        ;
        
    }
}

