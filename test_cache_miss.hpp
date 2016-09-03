#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <list>
#include <algorithm>
#include <string.h>

#include "common.h"

#include "diffclock.h"

namespace CacheMiss {
template <typename T>
class List {
public:
    List():first(nullptr), last(nullptr){}
    
    bool empty() const {
        return first == nullptr;
    }
    
    bool add(T data) {
        Node* newNode = new Node(data);
        if (first == nullptr) {
            last = first = newNode;
        } else {
            last->next = newNode;
            newNode->prev = last;
            newNode->next = nullptr;
            last = newNode;
        }
        return true;
    }
    
    bool remove(T data) {
        Node* temp = first;
        while (temp && temp->data != data)
            temp = temp->next;
        
        if (!temp) return false;
        if (temp == first) {
            Node* newFirst = first->next;
            delete first;
            if (newFirst) newFirst->prev = nullptr;
            first = newFirst;
        } else if (temp == last) {
            Node* newLast = last->prev;
            delete last;
            if (newLast) newLast->next = nullptr;
            last = newLast;
        } else {
            Node* left = temp->prev;
            Node* right = temp->next;
            delete temp;
            left->next = right;
            right->prev = left;
        }
        return true;
    }
private:
    struct Node {
        Node(T dataRhs):data(dataRhs), prev(nullptr), next(nullptr){}
        T data;
        Node* prev;
        Node* next;
    };
    
    Node* first;
    Node* last;
};

template <typename T>
class Vector {
public:
    Vector():ptr(nullptr), index(0), maxSize(0){}
    
    bool empty() const {
        return index == 0;
    }
    
    bool reset(int newSize) {
        delete[] ptr;
        ptr = new T[newSize];
        maxSize = newSize;
        return true;
    }
    
    bool add(T data) {
        if (index == maxSize) return false;
        ptr[index++] = data;
        return true;
    }
    
    bool remove(T data) {
        int remIndex = -1;
        for (int i = 0; i < maxSize; ++i) {
            if (ptr[i] == data)  {
                remIndex = i;
                break;
            };
        }
        if (remIndex == -1) return false;
        
        memmove(&ptr[remIndex], &ptr[remIndex + 1], maxSize - remIndex);
        --index;
        return true;
    }
private:
    T* ptr;
    int index;
    int maxSize;
};

    
inline constexpr size_t getTestSize() {
    return 81000;
}

template <typename T>
inline void eraseContainer(T& container) {
    { asm(""); }
    for (int i = 0; i < getTestSize(); ++i)
        container.remove(i);
}

template <typename T>
inline void eraseContainer(std::vector<T>& vec) {
    { asm(""); }
    for (int i = 0; i < getTestSize(); ++i) {
        auto iter = std::find(vec.begin(), vec.end(), i);
        vec.erase(iter);
    }
}

inline void test() {
    std::cout << "Testing cache misses ..." << std::endl;
    static std::vector<int> source;
    source.reserve(getTestSize());
    for (int i = 0; i < getTestSize(); ++i)
        source.push_back(i);
    
    std::random_shuffle(source.begin(), source.end());

    auto test0 = [&]{
        std::vector<int> vecStd;
        vecStd.reserve(source.size());
        for (int i = 0; i < getTestSize(); ++i)
            vecStd.push_back(source[i]);
        eraseContainer(vecStd);
    };
    
    auto test1 = [&]{
        std::list<int> listStd;
        for (int i = 0; i < getTestSize(); ++i)
            listStd.push_back(source[i]);
        eraseContainer(listStd);
    };
    
    auto test2 = [&]{
        Vector<int> vecCustom;
        vecCustom.reset((int)getTestSize());
        for (int i = 0; i < getTestSize(); ++i)
            vecCustom.add(source[i]);
        eraseContainer(vecCustom);
    };
    
    auto test3 = [&]{
        List<int> listCustom;
        for (int i = 0; i < getTestSize(); ++i)
            listCustom.add(source[i]);

        eraseContainer(listCustom);
    };
    
    //-----
    
    auto numRuns = 256;
    auto l1size = 32 * 1024 / sizeof(float) /*ints*/;
    auto size = l1size * 16;
    std::unique_ptr<float[]> array(new float[size]);
    std::fill(array.get(), array.get() + size, 0.f);
    
    auto test4 = [&] {
        for (int i=0; i<numRuns; i++)
            for (int j=0; j<size; j++)
                array[j] = 2.3*array[j]+1.2;
    };
    
    auto test5 = [&] {
        int blockstart = 0;
        const auto numBuckets = size/l1size;
        for (int b=0; b<numBuckets; b++) {
            for (int i=0; i<numRuns; i++) {
                for (int j=0; j<l1size; j++)
                    array[blockstart+j] = 2.3*array[blockstart+j]+1.2;
            }
            blockstart += l1size;
        }
    };
    
    ADD_BENCHMARK("CacheMiss \t std::vector", test0);
    ADD_BENCHMARK("CacheMiss \t std::list", test1);
    ADD_BENCHMARK("CacheMiss \t custom vector", test2);
    ADD_BENCHMARK("CacheMiss \t custom vector", test3);
    //ADD_BENCHMARK("CacheMiss \t linear add", test4);
    //ADD_BENCHMARK("CacheMiss \t bucket add", test5);

    benchpress::run_benchmarks(benchpress::options());
    
}
} //namespace CacheMiss
