#include <cassert>

#include "base/time/duration_pretty_printer.h"
#include "base/time/trace.h"
#include "dataset/img_clf_dataset.h"
#include "dataset/mnist.h"

using psyence::base::time::AsTightestSubseconds;
using psyence::base::time::Trace;
using psyence::dataset::ImgClfDataset;
using psyence::dataset::MNIST;

int main(int argc, char* argv[]) {
    assert(argc == 2);
    auto& dirname = argv[1];

    MNIST mnist_28x28;
    Trace trace;
    trace.Init();
    mnist_28x28.Load(dirname, &trace);

    trace.Enter("reduce_to_14x14");
    ImgClfDataset mnist_14x14;
    mnist_28x28.AvgPool({1, 2, 2}, &mnist_14x14);
    trace.Exit();

    auto pp = AsTightestSubseconds::New();
    string text;
    trace.Report(*pp, &text);
    delete pp;
    puts(text.data());
}
