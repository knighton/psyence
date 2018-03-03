#pragma once

#include "dataset/class.h"
#include "dataset/dataset.h"

namespace psyence {
namespace dataset {

// Image classification dataset split.
class ImgClfDatasetSplit : public DatasetSplit {
  public:
    const uint8_t* pixels() const { return pixels_; }
    Class num_classes() const { return num_classes_; }
    const Class* classes() const { return classes_; }

    virtual ~ImgClfDatasetSplit();

    virtual void InitImgClfDatasetSplit(
        size_t num_samples, const vector<size_t>& x_shape,
        const uint8_t* pixels, Class num_classes, const Class* classes);

    virtual void Get(size_t index, float* x, float* y) const;

    virtual void AvgPool(const vector<size_t>& pool_shape,
                         ImgClfDatasetSplit* out);

  protected:
    const uint8_t* pixels_{nullptr};
    Class num_classes_{0};
    const Class* classes_{nullptr};
};

// Image classification dataset.
class ImgClfDataset : public Dataset {
  public:
    Class num_classes() const { return num_classes_; }

    virtual ~ImgClfDataset();

    virtual void InitImgClfDataset(const vector<ImgClfDatasetSplit*>& splits);

    virtual void AvgPool(const vector<size_t>& pool_shape,
                         ImgClfDataset* out) const;

  protected:
    Class num_classes_{0};
};

}  // namespace dataset
}  // namespace psyence
