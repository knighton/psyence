#include "dataset.h"

#include <algorithm>
#include <cassert>
#include <unordered_map>

using std::shuffle;
using std::unordered_map;

namespace psyence {
namespace dataset {

DatasetSplit::~DatasetSplit() {
}

void DatasetSplit::InitDatasetSplit(
        size_t num_samples, const vector<size_t>& x_shape,
        const vector<size_t>& y_shape) {
    num_samples_ = num_samples;

    x_size_ = 1;
    for (auto& dim : x_shape) {
        assert(1 <= dim);
        x_size_ *= dim;
    }
    x_shape_ = x_shape;

    y_size_ = 1;
    for (auto& dim : y_shape) {
        assert(1 <= dim);
        y_size_ *= dim;
    }
    y_shape_ = y_shape;
}

bool DatasetSplit::Matches(const DatasetSplit& other) const {
    if (x_size_ != other.x_size_) {
        return false;
    }

    if (x_shape_.size() != other.x_shape_.size()) {
        return false;
    }
    for (size_t i = 0; i < x_shape_.size(); ++i) {
        if (x_shape_[i] != other.x_shape_[i]) {
            return false;
        }
    }

    if (y_size_ != other.y_size_) {
        return false;
    }

    if (y_shape_.size() != other.y_shape_.size()) {
        return false;
    }
    for (size_t i = 0; i < y_shape_.size(); ++i) {
        if (y_shape_[i] != other.y_shape_[i]) {
            return false;
        }
    }

    return true;
}

void Dataset::Free() {
    for (auto& split : splits_) {
        if (split) {
            delete split;
        }
    }
}

Dataset::~Dataset() {
    Free();
}

void Dataset::InitDataset(const vector<DatasetSplit*>& splits) {
    Free();
    splits_.clear();
    splits_.reserve(splits.size());
    num_samples_ = 0;
    for (auto& split : splits) {
        splits_.emplace_back(split);
        num_samples_ += split->num_samples();
    }
    for (size_t i = 1; i < splits_.size(); ++i) {
        assert(splits_[0]->Matches(*splits_[i]));
    }
}

void Dataset::Get(size_t split, size_t index_in_split, float* x,
                  float* y) const {
    splits_[split]->Get(index_in_split, x, y);
}

void Dataset::ShuffleSamples(
        const vector<size_t>& selected_splits, mt19937* rng,
        vector<pair<size_t, size_t>>* splits_indices) const {
    size_t count = 0;
    for (auto& split : selected_splits) {
        count += splits_[split]->num_samples();
    }
    if (splits_indices->size() != count) {
        splits_indices->resize(count);
    }
    size_t index = 0;
    for (auto& split : selected_splits) {
        count = splits_[split]->num_samples();
        for (size_t i = 0; i < count; ++i) {
            (*splits_indices)[index].first = split;
            (*splits_indices)[index].second = i;
            ++index;
        }
    }
    shuffle(splits_indices->begin(), splits_indices->end(), *rng);
}

void Dataset::ShuffleIntoBatches(
        const vector<size_t>& selected_splits, size_t batch_size, mt19937* rng,
        vector<size_t>* shuf_splits, vector<size_t>* shuf_indices) const {
    // Map each selected split to its index in the selected splits (and verify
    // uniqueness).
    unordered_map<size_t, size_t> split2idx_in_ss;
    for (size_t i = 0; i < selected_splits.size(); ++i) {
        auto& split = selected_splits[i];
        assert(split2idx_in_ss.find(split) == split2idx_in_ss.end());
        split2idx_in_ss[split] = i;
    }

    // Repopulate and shuffle shuf_splits.
    size_t total_num_batches = 0;
    for (auto& split : selected_splits) {
        total_num_batches += splits_[split]->num_samples() / batch_size;
    }
    if (shuf_splits->size() != total_num_batches) {
        shuf_splits->resize(total_num_batches);
    }
    size_t ptr = 0;
    for (auto& split : selected_splits) {
        auto num_batches = splits_[split]->num_samples() / batch_size;
        for (size_t i = 0; i < num_batches; ++i) {
            (*shuf_splits)[ptr++] = split;
        }
    }
    shuffle(shuf_splits->begin(), shuf_splits->end(), *rng);

    // Create a list of indices per selected split.
    vector<vector<size_t>> sample_indices_per_split;
    sample_indices_per_split.resize(selected_splits.size());
    for (size_t i = 0; i < selected_splits.size(); ++i) {
        auto& split = selected_splits[i];
        sample_indices_per_split.reserve(splits_[split]->num_samples());
        for (size_t j = 0; j < splits_[split]->num_samples(); ++j) {
            sample_indices_per_split[i].emplace_back(j);
        }
    }

    // Shuffle them.
    for (auto& sample_indices : sample_indices_per_split) {
        shuffle(sample_indices.begin(), sample_indices.end(), *rng);
    }

    // Pull the shuffled sample indices from the per-selected split vectors and
    // append them to shuf_indices.
    if (shuf_indices->size() != shuf_splits->size() * batch_size) {
        shuf_indices->resize(shuf_splits->size() * batch_size);
    }
    size_t to_ptr = 0;
    vector<size_t> ptrs;
    ptrs.resize(selected_splits.size());
    for (auto& split : *shuf_splits) {
        auto& index_in_ss = split2idx_in_ss.find(split)->second;
        auto& from_ptr = ptrs[index_in_ss];
        auto& sample_indices = sample_indices_per_split[index_in_ss];
        for (size_t i = 0; i < batch_size; ++i) {
            (*shuf_indices)[to_ptr++] = sample_indices[from_ptr++];
        }
    }
}

}  // namespace dataset
}  // namespace psyence
