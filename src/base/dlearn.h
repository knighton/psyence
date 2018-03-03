#pragma once

#include <cstdint>
#include <vector>

using std::vector;

namespace psyence {
namespace base {
namespace dlearn {

void AvgPool(const vector<size_t>& x_shape, const uint8_t* x,
             const vector<size_t>& pool_shape, uint8_t* y);

}  // namepsace dlearn
}  // namespace base
}  // namespace psyence
