#include <cassert>
#include <string>
#include <sys/stat.h>

#include "base/str/sprintf.h"
#include "base/time/duration_pretty_printer.h"
#include "base/time/trace.h"
#include "dataset/class.h"
#include "dataset/img_clf_dataset.h"
#include "dataset/mnist.h"

using psyence::base::str::sprintf::StringPrintf;
using psyence::base::time::AsMilliseconds;
using psyence::base::time::Trace;
using psyence::dataset::Class;
using psyence::dataset::ImgClfDataset;
using psyence::dataset::ImgClfDatasetSplit;
using psyence::dataset::MNIST;
using std::string;

namespace {

void ExportImages(size_t num_samples, size_t x_size, const uint8_t* pixels,
                  const string& filename, Trace* trace) {
    trace->Enter("images");
    FILE* out = fopen(filename.data(), "w");
    fwrite(pixels, sizeof(uint8_t), num_samples * x_size, out);
    fclose(out);
    trace->Exit();
}

void ExportClasses(size_t num_samples, const Class* classes,
                   const string& filename, Trace* trace) {
    trace->Enter("classes");
    FILE* out = fopen(filename.data(), "w");
    fwrite(classes, sizeof(Class), num_samples, out);
    fclose(out);
    trace->Exit();
}

void ExportReadme(const string& filename, Trace* trace) {
    trace->Enter("readme");
    FILE* out = fopen(filename.data(), "w");
    fprintf(out,
        "Reduced MNIST\n"
        "-------------\n"
        "\n"
        "train_images.bin: 60000x14x14 uint8_t pixels in row-major format\n"
        "train_classes.bin: 60000 uint16_t classes\n" 
        "\n"
        "test_images.bin: 10000x14x14 uint8_t pixels in row-major format\n"
        "test_classes.bin: 10000 uint16_t classes\n" 
    );
    fclose(out);
    trace->Exit();
}

void ExportSplit(const ImgClfDatasetSplit& split, const string& dirname, 
                 const string& split_name, Trace* trace) {
    trace->Enter(split_name);
    auto images_filename = StringPrintf(
        "%s/%s_images.bin", dirname.data(), split_name.data());
    ExportImages(split.num_samples(), split.x_size(), split.pixels(),
                 images_filename, trace);
    auto classes_filename = StringPrintf(
        "%s/%s_classes.bin", dirname.data(), split_name.data());
    ExportClasses(split.num_samples(), split.classes(), classes_filename,
                  trace);
    trace->Exit();
}

#define R(x) *reinterpret_cast<ImgClfDatasetSplit*>(x)

void Export(const ImgClfDataset& dataset, const string& dirname, Trace* trace) {
    trace->Enter("export");
    mkdir(dirname.data(), 0775);
    ExportReadme(dirname + "/README.txt", trace);
    ExportSplit(R(dataset.splits()[0]), dirname, "train", trace);
    ExportSplit(R(dataset.splits()[1]), dirname, "test", trace);
    trace->Exit();
}

}  // namespace

int main(int argc, char* argv[]) {
    assert(argc == 3);
    auto& mnist_dirname = argv[1];
    auto& reduced_mnist_dirname = argv[2];

    MNIST mnist_28x28;
    Trace trace;
    trace.Init();
    mnist_28x28.Load(mnist_dirname, &trace);

    trace.Enter("reduce_to_14x14");
    ImgClfDataset mnist_14x14;
    mnist_28x28.AvgPool({1, 2, 2}, &mnist_14x14);
    trace.Exit();

    Export(mnist_14x14, reduced_mnist_dirname, &trace);

    // auto pp = AsTightestSubseconds::New();
    auto pp = AsMilliseconds::New();
    string text;
    trace.Report(*pp, &text);
    delete pp;
    puts(text.data());
}
