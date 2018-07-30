#ifndef time_it_h_INCLUDED
#define time_it_h_INCLUDED

#include <chrono>

template <class Func>
std::chrono::milliseconds time_it(Func f)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    f();
    auto t2 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
}

#endif // time_it_h_INCLUDED

