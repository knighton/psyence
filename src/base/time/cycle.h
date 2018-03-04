#pragma once

#include <cstdint>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wundef"

namespace {

#include "ext/cycle/cycle.h"

}  // namespace

#pragma clang diagnostic pop

namespace psyence {
namespace base {
namespace time {
namespace cycle {

inline uint64_t ClockCycles() {
    return getticks();
}

}  // namespace cycles
}  // namespace time
}  // namespace base
}  // namespace psyence
