#pragma once

#include <thread>
#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace Threads {
    typedef std::lock_guard<std::mutex> LockGuard;
    
    class ThreadGuard {
        std::thread& t;
    public:
        ThreadGuard(std::thread& t_):t(t_){}
        ~ThreadGuard() {
            if(t.joinable()) {
                t.join();
            }
        }
        ThreadGuard(ThreadGuard const&)=delete;
        ThreadGuard& operator=(ThreadGuard const&)=delete;
    };
    
    void testHelloWorld() {
        std::function<void()> helloWorld = [](){std::cout << "Hello World";};
        std::thread t(helloWorld);
        ThreadGuard tg(t);
    }
    
    int getNumThreads() {
        const int hardwareThreads = static_cast<int>(std::thread::hardware_concurrency());
        return std::min(64, hardwareThreads !=0 ? hardwareThreads : 1);
    }
    
    template<typename Iterator, typename T>
    T testAccumulateParallel(Iterator first, Iterator last) {
        auto accumulateBlock = [](Iterator first, Iterator last, T& result) {
           result = std::accumulate(first, last, result);
        };
        
        const auto length = std::distance(first,last);
        T init = 0;
        if (length < 64)
            return std::accumulate(first, last, 0);
        
        const auto numThreads = getNumThreads();
        const auto blockSize = length/numThreads;
        std::vector<T> results(numThreads);
        std::vector<std::thread> threads(numThreads - 1);
        Iterator blockStart = first;
        
        for(int i = 0; i < (numThreads-1); ++i) {
            Iterator blockEnd = blockStart;
            std::advance(blockEnd, blockSize);
            threads[i] = std::thread(accumulateBlock,
                                    blockStart,
                                    blockEnd,
                                    std::ref(results[i]));
            blockStart = blockEnd;
        }
        accumulateBlock(blockStart, last, results[numThreads-1]);
        
        for (auto& t: threads) {
            t.join();
        }
        
        return std::accumulate(results.begin(), results.end(), init);

    }
    
    std::mutex m;
    template <typename T>
    void threadSafePushBack(std::vector<T>& container, const T& elem) {
        LockGuard guard(m);
        container.push_back(elem);
    }
    
    int getTestSize() {
        return 100_million;
    }
    
    void testFalseSharing() {
        struct Complex {
            float x, i;
            Complex(){ x = randomFloat(); i = randomFloat();}
        };
        
        std::unique_ptr<Complex[]> arr(new Complex[getTestSize()]);
        
        auto b = getTime();
        std::for_each(arr.get(), arr.get() + getTestSize(), [](Complex& complex) { complex.x += sinf(complex.x); complex.i += sinf(complex.i); });
        auto e = getTime();
        
        std::cout << "\t Single thread " << diffclock(e, b) << std::endl;
        
        b = getTime();
        auto sum = [&](bool real) {
            if (real) {
                std::for_each(arr.get(), arr.get() + getTestSize(), [](Complex& complex) { complex.x += sinf(complex.x); });
            } else {
                std::for_each(arr.get(), arr.get() + getTestSize(), [](Complex& complex) { complex.i += sinf(complex.i); });
            }
        };

        std::thread t0(sum, true);
        std::thread t1(sum, false);

        t0.join();
        t1.join();
        e = getTime();

        std::cout << "\t Multithread #0 " << diffclock(e, b) << std::endl;

        auto sum2 = [&](int begin, int end) {
            std::for_each(arr.get() + begin, arr.get() + end, [](Complex& complex) { complex.x += sinf(complex.x); complex.i += sinf(complex.i); });
        };
        b = getTime();
        std::thread t2(sum2, 0, getTestSize()/2);
        std::thread t3(sum2, getTestSize()/2, getTestSize());
        t2.join();
        t3.join();
        e = getTime();
        
        std::cout << "\t Multithread #1 " << diffclock(e, b) << std::endl;
    }
    
    void test() {
        std::cout << "Testing thread ..." << std::endl;
        testFalseSharing();
    }
}
