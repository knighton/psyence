#include "base/time/trace.h"
#include "dataset/mnist.h"

using psyence::base::time::Trace;
using psyence::dataset::MNIST;

int main() {
    auto dirname = "data/mnist/";
    MNIST dataset;
    Trace trace;
    trace.Init();
    dataset.Load(dirname, &trace);
}
