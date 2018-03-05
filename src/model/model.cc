#include "model.h"

#include <cstdlib>

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

    cor_.Init(num_neurons, correlation_momentum);
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
    // TODO
}

}  // namespace model
}  // namespace psyence
