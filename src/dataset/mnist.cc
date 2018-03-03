#include "mnist.h"

#include <algorithm>
#include <cassert>
#include <cstring>

#include "base/file.h"

using psyence::base::file::LoadFile;
using std::reverse;

namespace psyence {
namespace dataset {

void MNISTSplit::ParseMNISTImages(
        size_t num_bytes, const uint8_t* bytes, uint32_t magic,
        size_t num_samples, uint8_t** images) {
    assert(num_bytes == 16 + num_samples * 28 * 28);

    uint8_t head[16];
    memcpy(head, bytes, 16);

    reverse(head, head + 4);
    reverse(head + 4, head + 8);
    reverse(head + 8, head + 12);
    reverse(head + 12, head + 16);

    auto ints = reinterpret_cast<uint32_t*>(head);
    assert(ints[0] == magic);
    assert(ints[1] == num_samples);
    assert(ints[2] == 28);
    assert(ints[3] == 28);

    *images = new uint8_t[num_samples * 28 * 28];
    memcpy(*images, bytes + 4 * sizeof(uint32_t), num_samples * 28 * 28);
}

void MNISTSplit::LoadMNISTImages(
        const string& filename, uint32_t magic, size_t num_samples,
        uint8_t** images, Trace* trace) {
    trace->Enter("images");

    trace->Enter("read");
    size_t num_bytes;
    uint8_t* bytes;
    LoadFile(filename.c_str(), &num_bytes, &bytes);
    trace->Exit();

    trace->Enter("parse");
    ParseMNISTImages(num_bytes, bytes, magic, num_samples, images);
    trace->Exit();

    trace->Enter("free");
    delete [] bytes;
    trace->Exit();

    trace->Exit();
}

void MNISTSplit::ParseMNISTClasses(
        size_t num_bytes, const uint8_t* bytes, uint32_t magic,
        size_t num_samples, Class* num_classes, Class** classes) {
    assert(num_bytes == 8 + num_samples);

    uint8_t head[8];
    memcpy(head, bytes, 8);

    reverse(head, head + 4);
    reverse(head + 4, head + 8);

    auto ints = reinterpret_cast<uint32_t*>(head);
    assert(ints[0] == magic);
    assert(ints[1] == num_samples);

    *classes = new Class[num_samples];
    *num_classes = 0;
    for (size_t i = 0; i < num_samples; ++i) {
        auto& klass = bytes[8 + i];
        (*classes)[i] = klass;
        if (*num_classes < klass + 1) {
            *num_classes = klass + 1;
        }
    }
}

void MNISTSplit::LoadMNISTClasses(
        const string& filename, uint32_t magic, size_t num_samples,
        Class* num_classes, Class** classes, Trace* trace) {
    trace->Enter("classes");

    trace->Enter("read");
    size_t num_bytes;
    uint8_t* bytes;
    LoadFile(filename.c_str(), &num_bytes, &bytes);
    trace->Exit();

    trace->Enter("parse_bytes");
    ParseMNISTClasses(num_bytes, bytes, magic, num_samples, num_classes,
                      classes);
    trace->Exit();

    trace->Enter("free");
    delete [] bytes;
    trace->Exit();

    trace->Exit();
}

MNISTSplit::~MNISTSplit() {
}

void MNISTSplit::Init(size_t num_samples, const vector<size_t>& image_shape,
                      uint8_t* pixels, Class num_classes, Class* classes) {
    InitImgClfDatasetSplit(num_samples, image_shape, pixels, num_classes,
                           classes);
}

void MNISTSplit::Load(const string& images_filename,
                      const string& classes_filename, size_t num_samples,
                      Trace* trace) {
    uint8_t* pixels;
    LoadMNISTImages(images_filename, 0x803, num_samples, &pixels, trace);

    Class num_classes;
    Class* classes;
    LoadMNISTClasses(classes_filename, 0x801, num_samples, &num_classes,
                     &classes, trace);

    trace->Enter("init");
    vector<size_t> image_shape = {1, 28, 28};
    Init(num_samples, image_shape, pixels, num_classes, classes);
    trace->Exit();
}

MNIST::~MNIST() {
}

void MNIST::Init(const vector<MNISTSplit*>& splits) {
    auto cast = *reinterpret_cast<const vector<ImgClfDatasetSplit*>*>(&splits);
    InitImgClfDataset(cast);
}

void MNIST::Load(const string& dirname, Trace* trace) {
    trace->Enter("train_split");
    auto train_split = new MNISTSplit();
    auto images_filename = dirname + "train-images-idx3-ubyte";
    auto classes_filename = dirname + "train-labels-idx1-ubyte";
    train_split->Load(images_filename, classes_filename, 60000, trace);
    trace->Exit();

    trace->Enter("test_split");
    auto test_split = new MNISTSplit();
    images_filename = dirname + "t10k-images-idx3-ubyte";
    classes_filename = dirname + "t10k-labels-idx1-ubyte";
    test_split->Load(images_filename, classes_filename, 10000, trace);
    trace->Exit();

    trace->Enter("init");
    auto splits = {train_split, test_split};
    Init(splits);
    trace->Exit();
}

}  // namespace dataset
}  // namespace psyence
