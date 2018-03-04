#include "model.h"

#include <cstdlib>

namespace psyence {
namespace model {

void Model::Free() {
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

void Model::Init(Adapter* io, size_t num_neurons, size_t ticks_per_assoc,
                 size_t ticks_per_pred, float cor_momentum) {
    Free();
    io_ = io;
    num_neurons_ = num_neurons;
    ticks_per_assoc_ = ticks_per_assoc;
    ticks_per_pred_ = ticks_per_pred;
    cur_act_ = new float[num_neurons]();
    new_act_ = new float[num_neurons]();
    weight_ = new float[num_neurons * num_neurons];
    for (size_t i = 0; i < num_neurons * num_neurons; ++i) {
        auto x = static_cast<float>(rand()) / RAND_MAX;
        x = x * 2 - 1;
        weight_[i] = x / num_neurons;
    }
    cor_momentum_ = cor_momentum;
    cor_.Init(num_neurons, cor_momentum);
}

void Model::Associate(const float* x, const float* y_true) {
    io_->SetX(x, cur_act_);
    io_->SetY(y_true, cur_act_);
    for (size_t i = 0; i < ticks_per_assoc_; ++i) {
        Tick();
    }
}

void Model::Predict(const float* x, float* pred_means_per_tick,
                    float* pred_stds_per_tick) {
    io_->SetX(x, cur_act_);
    for (size_t i = 0; i < ticks_per_assoc_; ++i) {
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
