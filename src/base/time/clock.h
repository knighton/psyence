#pragma once

#include <chrono>
#include <cstdint>
#include <cstdio>

namespace psyence {
namespace base {
namespace time {
namespace clock {

// Get the current time, as nanoseconds since the epoch.
inline int64_t ClockNanosec() {
    auto time_point = std::chrono::high_resolution_clock::now();
    auto tse = time_point.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(tse).count();
}

// Get the current time, as floating point seconds since the epoch.
inline double ClockSec() {
    return ClockNanosec() / 1e9;
}

}  // namespace clock
}  // namespace time
}  // namespace base
}  // namespace psyence
