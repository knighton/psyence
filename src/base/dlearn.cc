#include "dlearn.h"

#include <cassert>

namespace psyence {
namespace base {
namespace dlearn {

void AvgPool(const vector<size_t>& x_shape, const uint8_t* x,
             const vector<size_t>& pool_shape, uint8_t* y) {
    // Validate input.
    assert(x_shape.size() == 3);
    for (auto& dim : x_shape) {
        assert(1 <= dim);
    }
    assert(pool_shape.size() == 3);
    size_t y_per_x = 1;
    for (auto& dim : pool_shape) {
        assert(1 <= dim);
        y_per_x *= dim;
    }

    // Get output shape.
    //
    // Currently we require it to divide evenly simply because we're lazy.
    vector<size_t> y_shape;
    y_shape.reserve(3);
    size_t y_size = 1;
    for (size_t i = 0; i < 3; ++i) {
        assert(x_shape[i] % pool_shape[i] == 0);
        y_shape.emplace_back(x_shape[i] / pool_shape[i]);
        y_size *= x_shape[i] / pool_shape[i];
    }

    // Populate a temporary buffer of a more spacious type to avoid u8 overflow.
    uint64_t* temp_y = new uint64_t[y_size]();
    for (size_t c = 0; c < x_shape[0]; ++c) {
        auto x_c_offset = c * x_shape[1] * x_shape[2];
        auto y_c_offset = c / pool_shape[0] * y_shape[1] * y_shape[2];
        for (size_t h = 0; h < x_shape[1]; ++h) {
            auto x_h_offset = x_c_offset + h * x_shape[2];
            auto y_h_offset =
                y_c_offset + h / pool_shape[1] * y_shape[2];
            for (size_t w = 0; w < x_shape[2]; ++w) {
                auto from_index = x_h_offset + w;
                auto to_index = y_h_offset + w / pool_shape[2];
                temp_y[to_index] += x[from_index];
            }
        }
    }

    // Now transfer to the output array.
    for (size_t c = 0; c < y_shape[0]; ++c) {
        size_t c_offset = c * y_shape[1] * y_shape[2];
        for (size_t h = 0; h < y_shape[1]; ++h) {
            auto h_offset = c_offset + h * y_shape[2];
            for (size_t w = 0; w < y_shape[2]; ++w) {
                auto index = h_offset + w;
                y[index] = static_cast<uint8_t>(temp_y[index] / y_per_x);
            }
        }
    }

    // Cleanup.
    delete [] temp_y;
}

}  // namespace dlearn
}  // namespace base
}  // namespace psyence
