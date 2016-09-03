#include <cmath>
#include <cstdio>
#include <chrono>
#include <functional>

#if defined(__GNUC__) && defined(HINT_CONDITIONS)
    #define likely(X) (__builtin_expect((X), 1))
    #define unlikely(X) (__builtin_expect((X), 0))
#else
    #define likely(X) (X)
    #define unlikely(X) (X)
#endif

inline std::chrono::time_point<std::chrono::system_clock> getTime() {
    return std::chrono::system_clock::now();
}

inline double diffclock(std::chrono::time_point<std::chrono::system_clock> end, std::chrono::time_point<std::chrono::system_clock> start){
    std::chrono::duration<double> elapsed_seconds = end-start;
    return elapsed_seconds.count();
}

inline void escape(void* ptr) {
#ifdef __clang__
    asm volatile("" : : "g"(ptr) : "memory");
#endif
}

inline void clobber() {
#ifdef __clang__
    asm volatile("" : : : "memory");
#endif
}

inline
int digitsLog(double x) {
    if (x < 10) return 1;
    return log(x + .1)/log(10.0) + 1.0;
}

inline
int digitsNaive(int x) {
    int result = 1;
    while (true) {
        x /= 10;
        if (x == 0)
            return result;
        result++;
    }
}

inline
int digitsNaiveFloat(double x) {
    int result = 1;
    while (true) {
        if (x < 10.0)
            return result;
        x /= 10.0;

        result++;
    }
}

inline
int digitsClever(int x) {
    int result = 1;
    int count = 10;
    while (true) {
        if (count > x)
            return result;
        result++;
        count *= 10;
    }
}

inline
int digitsHack1(int x) {
    int result = 1;
    while (true) {
        if (x < 10) return result;
        if (x < 100) return result + 1;
        if (x < 1000) return result + 2;
        if (x < 10000) return result + 3;
        result += 4;
        x /= 10000;
    }
}

void assertResults(int x) {
    for (int i = 0; i < x; ++i) {
        int results[] { digitsLog(i),
                        digitsNaive(i),
                        digitsNaiveFloat(i),
                        digitsClever(i),
                        digitsHack1(i),
                      };
        for (int j = 1; j < sizeof(results)/sizeof(int); ++j) {
            if (results[0] != results[j]) {
                int* ptr = nullptr;
                *ptr = 42;
            }
        }
        
    }
}

template<typename T>
void runTest(T x, std::function<int(T)> f) {
    for (int t = 0; t < 1; ++t) {
        for (T i = T(0); i < x; i = i + T(1)) {
            f(i);
        }
    }
    clobber();
}

int main(int argc, const char * argv[]) {
    const int testSize = 150'000'000;
    
    assertResults(testSize);
    
    auto b = getTime();
    runTest<double>(testSize, digitsLog);
    auto e = getTime();
    printf("digitsLog %f\n", diffclock(e, b));

    b = getTime();
    runTest<int>(testSize, digitsNaive);
    e = getTime();
    printf("digitsNaive %f\n", diffclock(e, b));
    
    b = getTime();
    runTest<double>(testSize, digitsNaiveFloat);
    e = getTime();
    printf("digitsNaiveFloat %f\n", diffclock(e, b));
    
    b = getTime();
    runTest<int>(testSize, digitsClever);
    e = getTime();
    printf("digitsClever %f\n", diffclock(e, b));
    
    b = getTime();
    runTest<int>(testSize, digitsHack1);
    e = getTime();
    printf("digitsHack1 %f\n", diffclock(e, b));

    return 0;
}

/*
 digitsLog 2.543432
 digitsNaive 2.301842
 digitsNaiveFloat 1.638176
 digitsClever 1.662899
 digitsHack1 1.287161
 Program ended with exit code: 0
 
 
 */
