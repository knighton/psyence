#pragma once

#include "base/stats/online_correlater.h"
#include "model/adapter.h"

using psyence::base::stats::OnlineCorrelater;
using psyence::model::Adapter;

namespace psyence {
namespace model {

class Model {
  public:
    // Free memory.
    ~Model();

    // Setup.
    //
    // Takes ownership of "io".
    void Init(Adapter* io, size_t num_neurons, float correlation_momentum);

    // Given X and Y, learn X -> Y.
    void Train(size_t num_ticks, const float* x, const float* y_true);

    // Given X, predict X -> Y.
    void Predict(size_t num_ticks, const float* x, float* pred_means_per_tick,
                 float* pred_stds_per_tick);

  private:
    // Free memory.
    void Free();

    // Perform one timestep.
    void Tick();

    // Inputs and outputs.
    Adapter* io_{nullptr};

    // The total number of neurons.
    size_t num_neurons_;

    // Neuron activations, and the buffer for computing the new activations.
    //
    // Shape: num_neurons_.
    float* cur_act_{nullptr};
    float* new_act_{nullptr};

    // Weights for each neuron feeding into each neuron.
    //
    // Shape: num_neurons_ * num_neurons_.
    float* weight_{nullptr};

    // How neurons correlate with each other in their activity.
    //
    // Used for gradually improving the wiring of the network.
    //
    // Shape: num_neurons_, num_neurons_ * num_neurons_.
    OnlineCorrelater cor_;
};

}  // namespace model
}  // namespace psyence
