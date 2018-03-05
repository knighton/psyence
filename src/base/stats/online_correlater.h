#pragma once

#include <cstddef>
#include <cstdio>

namespace psyence {
namespace base {
namespace stats {

// Online algorithm to correlate a list of variables.
class OnlineCorrelater {
  public:
    // Accessors.
    size_t num_variables() const { return num_variables_; }
    float momentum() const { return momentum_; }
    const float* means() const { return means_; }
    const float* stds() const { return stds_; }
    const float* cov() const { return cov_; }
    const float* cor() const { return cor_; }

    // Free memory.
    ~OnlineCorrelater();

    // Allocate space.
    void Init(size_t num_variables, float momentum);

    // Dump statistics to file.
    void Report(FILE* out) const;

    // Update statistics given one sample.
    void Update(const float* variables);

  private:
    // Free memory.
    void Free();

    // Number of variables.
    size_t num_variables_;

    // Momentum for updates.
    float momentum_;

    // Moving statistics for each variable:
    // * Mean.
    // * Standard deviation.
    //
    // Shape: num_variables_.
    float* means_{nullptr};
    float* stds_{nullptr};

    // Statistics on pairs of variables:
    // * Covariance matrix.
    // * Pearson correlation coefficient.
    //
    // Shape: num_variables_ * num_variables_.
    float* cov_{nullptr};
    float* cor_{nullptr};
};

}  // namespace stats
}  // namespace base
}  // namespace psyence
