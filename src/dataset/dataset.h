#pragma once

#include <cstddef>
#include <random>
#include <utility>
#include <vector>

using std::mt19937;
using std::pair;
using std::vector;

namespace psyence {
namespace dataset {

class DatasetSplit {
  public:
    // Accessors.
    size_t num_samples() const { return num_samples_; }
    size_t x_size() const { return x_size_; }
    const vector<size_t>& x_shape() const { return x_shape_; }
    size_t y_size() const { return y_size_; }
    const vector<size_t>& y_shape() const { return y_shape_; }

    // Free memory.
    virtual ~DatasetSplit();

    // Initialize shapes.
    virtual void InitDatasetSplit(
        size_t num_samples, const vector<size_t>& x_shape,
        const vector<size_t>& y_shape);

    // Get the sample at the given index.
    virtual void Get(size_t index, float* x, float* y) const = 0;

    // Verify shapes match between splits.
    virtual bool Matches(const DatasetSplit& other) const;

  protected:
    // Number of samples in this split.
    size_t num_samples_;

    // Dimensions of each x sample.
    size_t x_size_;
    vector<size_t> x_shape_;

    // Dimensions of each y sample.
    size_t y_size_;
    vector<size_t> y_shape_;
};

class Dataset {
  public:
    // Accessors.
    const vector<DatasetSplit*>& splits() const { return splits_; }
    size_t num_samples() const { return num_samples_; }

    size_t x_size() const { return splits_[0]->x_size(); }
    const vector<size_t>& x_shape() const { return splits_[0]->x_shape(); }
    size_t y_size() const { return splits_[0]->y_size(); }
    const vector<size_t>& y_shape() const { return splits_[0]->y_shape(); }

    // Free memory.
    virtual ~Dataset();

    // Initialize given the list of data splits (eg, train and test).
    //
    // Takes ownership of the splits.
    virtual void InitDataset(const vector<DatasetSplit*>& splits);

    // Get the sample from the given split at the given index.
    virtual void Get(size_t split, size_t index_in_split, float* x,
                     float* y) const;

    // Get a shuffle of the samples of the selected splits.
    //
    // Samples are listed one at a time, with the splits mixed together.
    //
    // Returns pairs of (split, index in split).
    virtual void ShuffleSamples(
        const vector<size_t>& selected_splits, mt19937* rng,
        vector<pair<size_t, size_t>>* splits_indices) const;

    // Get a shuffle of the samples of the selected splits into batches.
    //
    // Returns two flat vectors:
    // * shuf_splits: list of split indices
    // * shuf_indices: list of sample indices within the respective splits
    //
    // There are batch_size indices in shuf_indices for every one split index in
    // shuf_splits.
    //
    // Note that this drops a small number of different samples each sample in
    // order to make the batches even.
    virtual void ShuffleIntoBatches(
        const vector<size_t>& selected_splits, size_t batch_size, mt19937* rng,
        vector<size_t>* shuf_splits, vector<size_t>* shuf_indices) const;

  protected:
    // Free memory.
    virtual void Free();

    // The list of dataset splits (eg, train and test).
    vector<DatasetSplit*> splits_;

    // The number of samples across all splits.
    size_t num_samples_;
};

}  // namespace dataset
}  // namespace psyence
