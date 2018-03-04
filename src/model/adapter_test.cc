#include <cassert>
#include <cstdio>

#include "model/adapter.h"
#include "base/floats.h"

using psyence::base::floats::FloatEqual;
using psyence::model::Adapter;

int main() {
    float act_momentum = 0.5f;
    size_t x_repeats = 2;
    size_t x_dim = 5;
    size_t y_repeats = 3;
    size_t y_dim = 6;

    Adapter adapter;
    adapter.Init(act_momentum, x_repeats, x_dim, y_repeats, y_dim);

    assert(adapter.x_size() == x_repeats * x_dim);
    assert(adapter.y_size() == y_repeats * y_dim);
    assert(adapter.total_size() == adapter.x_size() + adapter.y_size());

    auto x = new float[x_dim];
    for (size_t i = 0; i < x_dim; ++i) {
        x[i] = i;
    }

    auto y = new float[y_dim];
    for (size_t i = 0; i < y_dim; ++i) {
        y[i] = i;
    }

    size_t num_neurons = adapter.total_size() + 100;
    auto acts = new float[num_neurons]();

    adapter.SetX(x, acts);
    for (size_t i = 0; i < x_repeats; ++i) {
        for (size_t j = 0; j < x_dim; ++j) {
            auto& act = acts[i * x_dim + j];
            assert(FloatEqual(act, x[j] * act_momentum, 1e-6f));
        }
    }
    for (size_t i = adapter.x_size(); i < num_neurons; ++i) {
        auto& act = acts[i];
        assert(FloatEqual(act, 0, 1e-6f));
    }

    adapter.SetY(y, acts);
    for (size_t i = 0; i < x_repeats; ++i) {
        for (size_t j = 0; j < x_dim; ++j) {
            auto& act = acts[i * x_dim + j];
            assert(FloatEqual(act, x[j] * act_momentum, 1e-6f));
        }
    }
    for (size_t i = 0; i < y_repeats; ++i) {
        for (size_t j = 0; j < y_dim; ++j) {
            auto& act = acts[adapter.x_size() + i * y_dim + j];
            assert(FloatEqual(act, y[j] * act_momentum, 1e-6f));
        }
    }
    for (size_t i = adapter.total_size(); i < num_neurons; ++i) {
        auto& act = acts[i];
        assert(FloatEqual(act, 0, 1e-6f));
    }

    auto pred_means = new float[y_dim];
    auto pred_stds = new float[y_dim];
    adapter.GetY(acts, pred_means, pred_stds);
    for (size_t i = 0; i < y_dim; ++i) {
        assert(FloatEqual(pred_means[i], i * act_momentum, 1e-6f));
        assert(FloatEqual(pred_stds[i], 0, 1e-6f));
    }
    delete [] pred_means;
    delete [] pred_stds;
}
