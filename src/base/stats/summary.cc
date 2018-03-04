#include "summary.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <string>

using std::isfinite;
using std::sort;
using std::string;

namespace psyence {
namespace base {
namespace stats {

void Summary::Free() {
    if (pcts_) {
        delete [] pcts_;
    }
}

Summary::~Summary() {
    Free();
}

void Summary::InitFromParams(
        size_t count, float min, float max, float sum, float std,
        size_t num_pcts, float* pcts) {
    count_ = count;
    min_ = min;
    max_ = max;
    sum_ = sum;
    std_ = std;
    num_pcts_ = num_pcts;
    pcts_ = pcts;
}

void Summary::InitFromData(size_t num_values, const float* values,
                           size_t num_pcts) {
    assert(num_values);

    count_ = num_values;

    sum_ = 0;
    sum_ = 0;
    for (size_t i = 0; i < num_values; ++i) {
        auto& x = values[i];
        assert(isfinite(x));
        sum_ += x;
        if (i) {
            if (x < min_) {
                min_ = x;
            }
            if (max_ < x) {
                max_ = x;
            }
        } else {
            min_ = x;
            max_ = x;
        }
    }

    auto mean = sum_ / num_values;
    std_ = 0;
    for (size_t i = 0; i < num_values; ++i) {
        auto& x = values[i];
        std_ += (mean - x) * (mean - x);
    }
    std_ /= num_values;
    std_ = static_cast<float>(sqrt(std_));

    float* sorted = new float[num_values];
    for (size_t i = 0; i < num_values; ++i) {
        sorted[i] = values[i];
    }
    sort(sorted, &sorted[num_values]);

    num_pcts_ = num_pcts;
    if (num_pcts) {
        pcts_ = new float[num_pcts];
        size_t pct_index = 0;
        for (size_t i = 0; i < num_pcts + 1; ++i) {
            size_t sorted_index = i * num_values / num_pcts;
            if (sorted_index == num_values) {
                sorted_index -= 1;
            }
            pcts_[pct_index++] = sorted[sorted_index];
        }
    }

    delete [] sorted;
}

void Summary::Report(FILE* out, size_t max_bar_len) const {
    auto mean = sum_ / count_;
    fprintf(out, "%zu values (min %.3f -- mean %.3f -- max %.3f) std %.3f.\n",
            count_, min_, mean, max_, std_);
    if (!max_bar_len) {
        return;
    }
    for (size_t i = 0; i < num_pcts_; ++i) {
        auto& pct = pcts_[i];
        size_t bar_len;
        if (pct <= 0) {
            bar_len = 0;
        } else {
            bar_len = static_cast<size_t>(pct * max_bar_len / max_);
        }
        fprintf(out, "%s\n", string(bar_len, '-').data());
    }
}

}  // namespace stats
}  // namespace base
}  // namespace psyence
