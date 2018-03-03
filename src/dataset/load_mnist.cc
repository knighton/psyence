#include <cassert>

#include "base/time/duration_pretty_printer.h"
#include "base/time/trace.h"
#include "dataset/mnist.h"

using psyence::base::time::AsTightestSubseconds;
using psyence::base::time::Trace;
using psyence::dataset::MNIST;

int main(int argc, char* argv[]) {
    assert(argc == 2);
    auto& dirname = argv[1];

    MNIST dataset;
    Trace trace;
    trace.Init();
    dataset.Load(dirname, &trace);

    auto pp = AsTightestSubseconds::New();
    string text;
    trace.Report(*pp, &text);
    delete pp;
    puts(text.data());
}
