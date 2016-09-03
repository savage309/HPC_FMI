#pragma once

#include <ctime>
#include <chrono>
#include "common.h"

inline std::chrono::time_point<std::chrono::system_clock> getTime() {
    return std::chrono::system_clock::now();
}

inline double diffclock(std::chrono::time_point<std::chrono::system_clock> end, std::chrono::time_point<std::chrono::system_clock> start){
      std::chrono::duration<double> elapsed_seconds = end-start;
    return elapsed_seconds.count();
}
