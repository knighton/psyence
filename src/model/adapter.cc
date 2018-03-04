#include "adapter.h"

#include <cmath>
#include <cstdio>

#include "base/momentum.h"

using psyence::base::momentum::MomUpdate;

namespace psyence {
namespace model {

void Adapter::Init(float act_momentum, size_t x_repeats, size_t x_dim,
                   size_t y_repeats, size_t y_dim) {
    act_momentum_ = act_momentum;

    x_repeats_ = x_repeats;
    x_dim_ = x_dim;

    y_repeats_ = y_repeats;
    y_dim_ = y_dim;

    x_size_ = x_repeats * x_dim;
    y_size_ = y_repeats * y_dim;
    total_size_ = x_size_ + y_size_;
}

void Adapter::SetX(const float* x, float* acts) const {
    for (size_t i = 0; i < x_repeats_; ++i) {
        for (size_t j = 0; j < x_dim_; ++j) {
            auto& act = acts[i * x_dim_ + j];
            act = MomUpdate(act_momentum_, act, x[j]);
        }
    }
}

void Adapter::SetY(const float* y, float* acts) const {
    for (size_t i = 0; i < y_repeats_; ++i) {
        for (size_t j = 0; j < y_dim_; ++j) {
            auto& act = acts[x_size_ + i * y_dim_ + j];
            act = MomUpdate(act_momentum_, act, y[j]);
        }
    }
}

void Adapter::GetY(const float* acts, float* pred_means,
                   float* pred_stds) const {
    auto ff = new float[y_repeats_ * y_dim_];
    for (size_t i = 0; i < y_repeats_; ++i) {
        for (size_t j = 0; j < y_dim_; ++j) {
            auto& act = acts[x_size_ + i * y_dim_ + j];
            ff[j * y_repeats_ + i] = act;
        }
    }
    for (size_t i = 0; i < y_dim_; ++i) {
        float sum = 0;
        for (size_t j = 0; j < y_repeats_; ++j) {
            auto& f = ff[i * y_repeats_ + j];
            sum += f;
        }
        auto mean = sum / y_repeats_;
        float std = 0;
        for (size_t j = 0; j < y_repeats_; ++j) {
            auto& f = ff[i * y_repeats_ + j];
            std += (f - mean) * (f - mean);
        }
        std = static_cast<float>(sqrt(std));
        std /= y_repeats_;
        pred_means[i] = mean;
        pred_stds[i] = std;
    }
}

}  // namespace model
}  // namespace psyence
