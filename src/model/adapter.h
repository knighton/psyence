#pragma once

#include <cstddef>

namespace psyence {
namespace model {

// Object which converts images and categories <-> activations.
class Adapter {
  public:
    // Accessors.
    float act_momentum() const { return act_momentum_; }
    size_t x_repeats() const { return x_repeats_; }
    size_t x_dim() const { return x_dim_; }
    size_t y_repeats() const { return y_repeats_; }
    size_t y_dim() const { return y_dim_; }
    size_t x_size() const { return x_size_; }
    size_t y_size() const { return y_size_; }
    size_t total_size() const { return total_size_; }

    // Set knobs.
    void Init(float act_momentum, size_t x_repeats, size_t x_dim,
              size_t y_repeats, size_t y_dim);

    // Project X into the neurons, given the momentum.
    void SetX(const float* x, float* acts) const;

    // Project Y into the neurons, given the momentum.
    void SetY(const float* y, float* acts) const;

    // Probe Y out of the neurons, gathering mean/std over the repeats.
    void GetY(const float* acts, float* pred_means, float* pred_stds) const;

  private:
    // Momentum of the activations when setting new activity.
    float act_momentum_;

    // X: "x_repeats" repeats of "x_dim" floats.
    size_t x_repeats_;
    size_t x_dim_;

    // Y: "y_repeats" repeats of "y_dim" floats.
    size_t y_repeats_;
    size_t y_dim_;

    // Derived dimensionality of X and Y projections into the net.
    size_t x_size_;
    size_t y_size_;
    size_t total_size_;
};

}  // namespace model
}  // namespace psyence
