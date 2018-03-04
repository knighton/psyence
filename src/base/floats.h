#pragma once

namespace psyence {
namespace base {
namespace floats {

inline bool FloatEqual(float a, float b, float eps) {
    auto x = a - b;
    return -eps < x && x < eps;
}

}  // namespace floats
}  // namespace base
}  // namespace psyence
