#include "trainer.h"

#include <cassert>
#include <chrono>
#include <cstdio>
#include <sys/stat.h>
#include <thread>

#include "base/collection/json.h"

using namespace std::chrono_literals;
using psyence::base::collection::json;

namespace psyence {
namespace model {

void Trainer::Free() {
    if (x_) {
        delete [] x_;
    }
    if (y_true_) {
        delete [] y_true_;
    }
    if (pred_means_per_tick_) {
        delete [] pred_means_per_tick_;
    }
    if (pred_stds_per_tick_) {
        delete [] pred_stds_per_tick_;
    }
}

Trainer::~Trainer() {
    Free();
}

void Trainer::Init(const Dataset* dataset, size_t train_split,
                   size_t test_split, Model* model, size_t ticks_per_train,
                   size_t ticks_per_predict, const string& eval_filename) {
    lock_.lock();

    Free();

    CROW_ROUTE(app_, "/")([]() {
        return "Hello world!";
    });

    CROW_ROUTE(app_, "/stop")([this]() {
        Stop();
        this->app_.stop();
        return "";
    });

    stop_requested_ = false;

    dataset_ = dataset;
    train_split_ = train_split;
    test_split_ = test_split;
    splits_ = {train_split, test_split};

    model_ = model;
    ticks_per_train_ = ticks_per_train;
    ticks_per_predict_ = ticks_per_predict;
    eval_filename_ = eval_filename;

    {
        struct stat buffer;
        if (!stat(eval_filename.data(), &buffer)) {
            assert(!remove(eval_filename.c_str()));
        }
    }

    iter_ = 0;
    dataset_->ShuffleSamples(splits_, &rng_, &epoch_);

    x_ = new float[dataset->x_size()];
    y_true_ = new float[dataset->y_size()];
    pred_means_per_tick_ = new float[dataset->y_size() * ticks_per_predict];
    pred_stds_per_tick_ = new float[dataset->y_size() * ticks_per_predict];

    rng_ = mt19937(rd_());

    auto eval_meta_filename = eval_filename + ".meta.json";
    auto eval_meta_file = fopen(eval_meta_filename.data(), "w");
    SaveEvalMetadata(eval_meta_file);

    lock_.unlock();
}

void Trainer::SaveEvalMetadata(FILE* eval_meta_file) const {
    json x = {
        {"ticks_per_predict", ticks_per_predict_},
        {"y_size", dataset_->y_size()},
    };
    fprintf(eval_meta_file, "%s\n", x.dump().data());
}

void Trainer::SaveEvalData(FILE* eval_file) const {
    fwrite(y_true_, sizeof(float), dataset_->y_size(), eval_file);
    auto count = dataset_->y_size() * ticks_per_predict_;
    fwrite(pred_means_per_tick_, sizeof(float), count, eval_file);
    fwrite(pred_stds_per_tick_, sizeof(float), count, eval_file);
}

void Trainer::RunIteration(FILE* eval_file) {
    // Get the split and sample index within that split.
    auto index_in_epoch = iter_ % epoch_.size();
    auto& pair = epoch_[index_in_epoch];
    auto& split = pair.first;
    auto& index_in_split = pair.second;

    // Load the sample into our internal preallocated buffers.
    dataset_->Get(split, index_in_split, x_, y_true_);

    // Run it through the model.
    if (split) {
        // Predict Y given X, getting for each tick both the mean and standard
        // deviation of each output float (across Y repeats).
        model_->Predict(ticks_per_predict_, x_, pred_means_per_tick_,
                        pred_stds_per_tick_);

        // Then, append the resulting floats to file for later analysis.
        SaveEvalData(eval_file);
    } else {
        // Supposedly learn X -> Y.
        model_->Train(ticks_per_train_, x_, y_true_);
    }

    // Increment our iteration index.
    ++iter_;

    // Do a shuffle if we finished the epoch.
    if (iter_ % epoch_.size() == 0) {
        dataset_->ShuffleSamples(splits_, &rng_, &epoch_);
    }
}

void Trainer::ServerThread(uint16_t port) {
    app_.loglevel(crow::LogLevel::Warning).port(port).multithreaded().run();
}

size_t Trainer::Start(size_t num_iter, uint16_t port) {
    std::thread(&Trainer::ServerThread, this, port).detach();
    FILE* eval_file = fopen(eval_filename_.data(), "a");
    for (size_t i = 0; i < num_iter; ++i) {
        lock_.lock();
        if (stop_requested_) {
            fclose(eval_file);
            stop_requested_ = false;
            app_.stop();
            std::this_thread::sleep_for(10ms);
            lock_.unlock();
            return i;
        }
        RunIteration(eval_file);
        lock_.unlock();
    }
    fclose(eval_file);
    return num_iter;
}

void Trainer::Stop() {
    lock_.lock();
    stop_requested_ = true;
    lock_.unlock();
}

}  // namespace model
}  // namespace psyence
