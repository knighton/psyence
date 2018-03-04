#pragma once

#include <cstddef>
#include <cstdio>

namespace psyence {
namespace base {
namespace stats {

// Summary statistics object.
class Summary {
  public:
    // Accessors.
    size_t count() const { return count_; }
    float min() const { return min_; }
    float max() const { return max_; }
    float sum() const { return sum_; }
    float mean() const { return sum_ / count_; }
    float std() const { return std_; }
    size_t num_pcts() const { return num_pcts_; }
    const float* pcts() const { return pcts_; }

    // Free memory.
    ~Summary();

    // Init from parameters.
    void InitFromParams(size_t count, float min, float max, float sum,
                        float std, size_t num_pcts, float* pcts);

    // Init from data.
    void InitFromData(size_t num_values, const float* values, size_t num_pcts);

    // Print a summary to the given file handle.
    void Report(FILE* out, size_t bar_max_len) const;

  private:
    // Free memory.
    void Free();

    // Parameters.
    size_t count_;
    float min_;
    float max_;
    float sum_;
    float std_;
    size_t num_pcts_;
    float* pcts_{nullptr};
};

}  // namespace stats
}  // namespace base
}  // namespace psyence
