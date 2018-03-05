#pragma once

#include <mutex>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "base/server/crow.h"
#include "dataset/dataset.h"
#include "model/model.h"

using psyence::base::server::crow::SimpleApp;
using psyence::dataset::Dataset;
using psyence::model::Model;
using std::mt19937;
using std::mutex;
using std::pair;
using std::random_device;
using std::string;
using std::vector;

namespace psyence {
namespace model {

class Trainer {
  public:
    // Free memory.
    ~Trainer();

    // Setup.
    //
    // Set the dataset and model, and execution parameters.
    void Init(const Dataset* dataset, size_t train_split, size_t test_split,
              Model* model, size_t ticks_per_train, size_t ticks_per_predict,
              const string& eval_filename);

    // Train a model against a dataset.
    //
    // Executes the training/validation loop for num_iter iterations, unless
    // stopped prematurely via Stop().  Returns the number of iterations that
    // were performed.
    //
    // While running it runs a webserver that serves a simple API on the
    // specified port, having to do with dumping state and stopping training.
    size_t Start(size_t num_iter, uint16_t port);

    // Stop training execution.
    //
    // Sets the internal stop_requested_ flag that is checked at the start of
    // each iteration.
    //
    // It blocks on lock_ waiting to do this, but it's a rare, manual operation
    // so more fine-grained locking wouldn't matter.
    void Stop();

  private:
    // Save the dimensions of the evaluation data to file.
    //
    // This is so the evaluation data file can be read correctly.
    void SaveEvalMetadata(FILE* eval_meta_file) const;

    // Append one sample's worth of validation results to the file.
    //
    // Stores the grouth truth and predicted weights for later analysis.
    // Assumes it holds lock_ (do not overwrite new results into the Y buffers
    // while using them for fwrite).
    void SaveEvalData(FILE* eval_file) const;

    // Execute one iteration.
    //
    // Called by Start().  Assumes it holds lock_.
    void RunIteration(FILE* eval_file);

    // Thread that spawns a webserver in the background while it's training.
    void ServerThread(uint16_t port);

    // Free memory.
    void Free();

    // Either train or handle a webserver request (dump, stop training).
    mutex lock_;

    // Webserver listening for stop requests while running.
    SimpleApp app_;

    // Allow other threads to request us to stop training.
    bool stop_requested_;

    // Dataset.
    const Dataset* dataset_;
    size_t train_split_;
    size_t test_split_;
    vector<size_t> splits_;

    // Model and execution config.
    Model* model_;
    size_t ticks_per_train_;
    size_t ticks_per_predict_;
    string eval_filename_;

    // Execution progress.
    size_t iter_;
    vector<pair<size_t, size_t>> epoch_;

    // The current sample's floats.
    float* x_{nullptr};
    float* y_true_{nullptr};
    float* pred_means_per_tick_{nullptr};
    float* pred_stds_per_tick_{nullptr};

    // For shuffling.
    random_device rd_;
    mt19937 rng_;
};

}  // namespace model
}  // namespace psyence
