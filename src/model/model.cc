#include "model.h"

#include <cmath>
#include <cstdlib>

#include "base/cxx.h"
#include "base/stats/summary.h"

using psyence::base::stats::Summary;

namespace psyence {
namespace model {

void Model::Free() {
    if (io_) {
        delete io_;
    }
    if (cur_act_) {
        delete [] cur_act_;
    }
    if (new_act_) {
        delete [] new_act_;
    }
    if (weight_) {
        delete [] weight_;
    }
}

Model::~Model() {
    Free();
}

void Model::Init(Adapter* io, size_t num_neurons, float correlation_momentum) {
    Free();

    io_ = io;
    num_neurons_ = num_neurons;

    cur_act_ = new float[num_neurons]();
    new_act_ = new float[num_neurons]();

    weight_ = new float[num_neurons * num_neurons];
    for (size_t i = 0; i < num_neurons * num_neurons; ++i) {
        auto x = static_cast<float>(rand()) / RAND_MAX;
        x = x * 2 - 1;
        weight_[i] = x / num_neurons;
    }

    correlater_.Init(num_neurons, correlation_momentum);
}

void Model::Train(size_t num_ticks, const float* x, const float* y_true) {
    io_->SetX(x, cur_act_);
    io_->SetY(y_true, cur_act_);
    for (size_t i = 0; i < num_ticks; ++i) {
        Tick();
    }
}

void Model::Predict(size_t num_ticks, const float* x,
                    float* pred_means_per_tick, float* pred_stds_per_tick) {
    io_->SetX(x, cur_act_);
    for (size_t i = 0; i < num_ticks; ++i) {
        Tick();
        io_->GetY(cur_act_, &pred_means_per_tick[i * io_->x_dim()],
                 &pred_stds_per_tick[i * io_->x_dim()]);
    }
}

void Model::Tick() {
    for (size_t i = 0; i < num_neurons_; ++i) {
        new_act_[i] = 0;
    }

    float sum_of_sums = 0;
    for (size_t i = 0; i < num_neurons_; ++i) {
        float sum = 0;
        for (size_t j = 0; j < num_neurons_; ++j) {
            auto& act = cur_act_[j];
            auto& weight = weight_[i * num_neurons_ + j];
            sum += weight * act;
        }
        new_act_[i] = sum;
        sum_of_sums += sum;
    }
    auto mean = sum_of_sums / num_neurons_;
    float x = 0;
    for (size_t i = 0; i < num_neurons_; ++i) {
        auto& act = new_act_[i];
        x += (mean - act) * (mean - act);
        act -= mean;
    }
    auto std = static_cast<float>(sqrt(x / num_neurons_));
    for (size_t i = 0; i < num_neurons_; ++i) {
        auto& act = new_act_[i];
        act /= std;
    }

#if 0
    printf("\n\n\n\n");
    printf("    TICK\n\n");

    Summary stats;
    stats.InitFromData(num_neurons_, new_act_, 20);
    stats.Report("new_act", 80, stdout);

    stats.InitFromData(num_neurons_ * num_neurons_, weight_, 20);
    stats.Report("weight", 80, stdout);

    correlater_.Update(new_act_);
    correlater_.Report(stdout);

    printf("\n\n");
#endif

    correlater_.Update(new_act_);

    for (size_t i = 0; i < num_neurons_; ++i) {
        for (size_t j = 0; j < num_neurons_; ++j) {
            auto& weight = weight_[i * num_neurons_ + j];
            auto& cor = correlater_.cor()[i * num_neurons_ + j];
            weight += cor;
        }
    }

    auto tmp = cur_act_;
    cur_act_ = new_act_;
    new_act_ = tmp;
}

}  // namespace model
}  // namespace psyence
