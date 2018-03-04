#pragma once

namespace psyence {
namespace base {
namespace momentum {

inline float MomUpdate(float momentum, float x, float new_x) {
    return momentum * x + (1 - momentum) * new_x;
}

}  // namespace momentum
}  // namespace base
}  // namespace psyence
