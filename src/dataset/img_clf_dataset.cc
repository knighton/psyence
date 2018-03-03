#include "img_clf_dataset.h"

#include <cassert>
#include <cstring>

#include "base/dlearn.h"

using psyence::base::dlearn::AvgPool;

namespace psyence {
namespace dataset {

ImgClfDatasetSplit::~ImgClfDatasetSplit() {
    if (pixels_) {
        delete [] pixels_;
    }
    if (classes_) {
        delete [] classes_;
    }
}

void ImgClfDatasetSplit::InitImgClfDatasetSplit(
        size_t num_samples, const vector<size_t>& x_shape,
        const uint8_t* pixels, Class num_classes, const Class* classes) {
    vector<size_t> y_shape = {num_classes};
    InitDatasetSplit(num_samples, x_shape, y_shape);
    pixels_ = pixels;
    num_classes_ = num_classes;
    classes_ = classes;
}

void ImgClfDatasetSplit::Get(size_t index, float* x, float* y) const {
    assert(index < num_samples_);
    size_t offset = index * x_size_;
    for (size_t i = 0; i < x_size_; ++i) {
        x[i] = pixels_[offset + i] / 255.0f;
    }
    for (size_t i = 0; i < num_classes_; ++i) {
        y[i] = 0;
    }
    auto klass = classes_[index];
    y[klass] = 1;
}

void ImgClfDatasetSplit::AvgPool(
        const vector<size_t>& pool_shape, ImgClfDatasetSplit* out) {
    size_t new_x_size = 1;
    vector<size_t> new_x_shape;
    new_x_shape.reserve(3);
    assert(pool_shape.size() == 3);
    for (size_t i = 0; i < 3; ++i) {
        assert(1 <= pool_shape[i]);
        assert(x_shape_[i] % pool_shape[i] == 0);
        new_x_shape.emplace_back(x_shape_[i] / pool_shape[i]);
        new_x_size *= x_shape_[i] / pool_shape[i];
    }
    auto new_pixels = new uint8_t[num_samples_ * new_x_size];
    for (size_t i = 0; i < num_samples_; ++i) {
        auto x = pixels_ + i * x_size_;
        auto y = new_pixels + i * new_x_size;
        ::AvgPool(x_shape_, x, pool_shape, y);
    }
    auto new_classes = new Class[num_samples_];
    memcpy(new_classes, classes_, num_samples_ * sizeof(Class));
    out->InitImgClfDatasetSplit(num_samples_, new_x_shape, new_pixels,
                                num_classes_, new_classes);
}

ImgClfDataset::~ImgClfDataset() {
}

void ImgClfDataset::InitImgClfDataset(
        const vector<ImgClfDatasetSplit*>& splits) {
    num_classes_ = 0;
    for (auto& split : splits) {
        auto z = split->num_classes();
        if (num_classes_ < z) {
            num_classes_ = z;
        }
    }
    InitDataset(reinterpret_cast<const vector<DatasetSplit*>&>(splits));
}

void ImgClfDataset::AvgPool(const vector<size_t>& pool_shape,
                            ImgClfDataset* out) const {
    vector<ImgClfDatasetSplit*> out_splits;
    out_splits.reserve(splits_.size());
    for (size_t i = 0; i < splits_.size(); ++i) {
        auto out_split = new ImgClfDatasetSplit();
        auto in_split = reinterpret_cast<ImgClfDatasetSplit*>(splits_[i]);
        in_split->AvgPool(pool_shape, out_split);
        out_splits.emplace_back(out_split);
    }
    out->InitImgClfDataset(out_splits);
}

}  // namespace dataset
}  // namespace psyence
