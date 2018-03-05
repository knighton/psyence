#include "online_correlater.h"

#include <cassert>
#include <cmath>

#include "base/stats/summary.h"
#include "base/momentum.h"

using psyence::base::momentum::MomUpdate;
using std::isfinite;

namespace psyence {
namespace base {
namespace stats {

void OnlineCorrelater::Free() {
    if (means_) {
        delete [] means_;
    }
    if (stds_) {
        delete [] stds_;
    }
    if (cov_) {
        delete [] cov_;
    }
    if (cor_) {
        delete [] cor_;
    }
}

OnlineCorrelater::~OnlineCorrelater() {
    Free();
}

void OnlineCorrelater::Init(size_t num_variables, float momentum) {
    Free();
    num_variables_ = num_variables;
    momentum_ = momentum;
    means_ = new float[num_variables]();
    stds_ = new float[num_variables]();
    for (size_t i = 0; i < num_variables; ++i) {
        stds_[i] = 1;
    }
    cov_ = new float[num_variables * num_variables]();
    cor_ = new float[num_variables * num_variables]();
}

void OnlineCorrelater::Report(FILE* out) const {
    size_t max_bar_len = 80;

    printf("\n");

    printf(""
"    +---------------------------------------------------------------------+\n"
"    | Correlater                                                          |\n"
"    +---------------------------------------------------------------------+\n"
    );

    printf("\n");

    Summary x;
    x.InitFromData(num_variables_, means_, 10);
    x.Report("means", max_bar_len, out);

    x.InitFromData(num_variables_, stds_, 10);
    x.Report("stds", max_bar_len, out);

    x.InitFromData(num_variables_ * num_variables_, cov_, 10);
    x.Report("cov", max_bar_len, out);

    x.InitFromData(num_variables_ * num_variables_, cor_, 10);
    x.Report("cor", max_bar_len, out);

    printf("\n");
}

void OnlineCorrelater::Update(const float* variables) {
    for (size_t i = 0; i < num_variables_; ++i) {
        auto x_gap = variables[i] - means_[i];
        for (size_t j = 0; j < num_variables_; ++j) {
            auto y_gap = variables[j] - means_[j];
            auto& cov = cov_[i * num_variables_ + j];
            cov = MomUpdate(momentum_, cov, x_gap * y_gap / num_variables_);
            auto& cor = cor_[i * num_variables_ + j];
            auto std_i = stds_[i] + 1e-3f;
            auto std_j = stds_[j] + 1e-3f;
            auto sample_correl = cov / (std_i * std_j);
            cor = MomUpdate(momentum_, cor, sample_correl);
        }
    }
    for (size_t i = 0; i < num_variables_; ++i) {
        auto& x = variables[i];
        assert(isfinite(x));
        auto& mean = means_[i];
        auto& std = stds_[i];
        auto sample_std = static_cast<float>(sqrt((mean - x) * (mean - x)));
        mean = MomUpdate(momentum_, mean, x);
        std = MomUpdate(momentum_, std, sample_std);
    }
}

}  // namespace stats
}  // namespace base
}  // namespace psyence
