#ifndef TIME_HH
#define TIME_HH

#include <chrono>
using namespace std::chrono_literals;

namespace ps {

using hr = std::chrono::high_resolution_clock;
using Time = decltype(hr::now());
using Duration = decltype(hr::now() - hr::now());

}

#endif //TIME_HH
