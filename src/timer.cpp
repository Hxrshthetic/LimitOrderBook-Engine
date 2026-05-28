#include "timer.hpp"

#ifdef _WIN32
#include <windows.h>

double get_time() {
    static LARGE_INTEGER freq;
    static int init = []() {
        QueryPerformanceFrequency(&freq);
        return 0;
    }();
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    return static_cast<double>(count.QuadPart) * 1000000.0 / static_cast<double>(freq.QuadPart);
}
#else
#include <chrono>
double get_time() {
    return std::chrono::duration<double, std::micro>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}
#endif