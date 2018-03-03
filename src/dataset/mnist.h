#pragma once

#include <string>

#include "base/time/trace.h"
#include "dataset/class.h"
#include "dataset/img_clf_dataset.h"

using psyence::base::time::Trace;
using std::string;

namespace psyence {
namespace dataset {

// One split of MNIST.
class MNISTSplit : public ImgClfDatasetSplit {
  public:
    // Load MNIST images.
    static void ParseMNISTImages(
        size_t num_bytes, const uint8_t* bytes, uint32_t magic,
        size_t num_samples, uint8_t** images);
    static void LoadMNISTImages(
        const string& filename, uint32_t magic, size_t num_samples,
        uint8_t** images, Trace* trace);

    // Load MNIST classes.
    static void ParseMNISTClasses(
        size_t num_bytes, const uint8_t* bytes, uint32_t magic,
        size_t num_samples, Class* num_classes, Class** classes);
    static void LoadMNISTClasses(
        const string& filename, uint32_t magic, size_t num_samples,
        Class* num_classes, Class** classes, Trace* trace);

    // Free memory.
    ~MNISTSplit();

    // Initialize.
    void Init(size_t num_samples, const vector<size_t>& image_shape,
              uint8_t* pixels, Class num_classes, Class* classes);

    // Load a split of MNIST from the files.
    void Load(const string& images_filename, const string& classes_filename,
              size_t num_samples, Trace* trace);
};

// MNIST handwritten digits dataset.
class MNIST : public ImgClfDataset {
  public:
    // Free memory.
    virtual ~MNIST();

    // Initialize.
    void Init(const vector<MNISTSplit*>& splits);

    // Load the dataset from a data directory.
    void Load(const string& dirname, Trace* trace);
};

}  // namespace dataset
}  // namespace psyence
