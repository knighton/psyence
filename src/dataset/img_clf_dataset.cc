#include "img_clf_dataset.h"

#include <cassert>
#include <cstring>

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

}  // namespace dataset
}  // namespace psyence
