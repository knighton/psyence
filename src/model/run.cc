#include <cstdio>
#include <gflags/gflags.h>
#include <string>

#include "base/time/trace.h"
#include "dataset/img_clf_dataset.h"
#include "dataset/mnist.h"
#include "model/adapter.h"
#include "model/model.h"
#include "model/trainer.h"

using psyence::base::time::Trace;
using psyence::dataset::ImgClfDataset;
using psyence::dataset::MNIST;
using psyence::model::Adapter;
using psyence::model::Model;
using psyence::model::Trainer;
using std::string;

// Dataset flags.
DEFINE_string(mnist_dir, "data/mnist/", "MNIST dataset directory");
DEFINE_uint64(train_split, 0, "Index of the MNIST split to train on");
DEFINE_uint64(test_split, 1, "Index of the MNIST split to test on");

// Adapter flags.
DEFINE_double(act_momentum, 0.5, "What fraction of neurons' activation values "
              "to keep when blending in new activation values from the world");
DEFINE_uint64(x_repeats, 1, "Number of consecutive repeats of X into the "
              "neurons");
DEFINE_uint64(y_repeats, 1, "Number of consecutive repeats of Y into the "
              "neurons");

// Model flags.
DEFINE_uint64(num_neurons, 512, "Total number of neurons");
DEFINE_double(correlation_momentum, 0.99, "Momentum of inter-neuron "
              "correlation statistics.");

// Trainer flags.
DEFINE_uint64(ticks_per_train, 4, "Number of cycles taken to process each "
              "training sample");
DEFINE_uint64(ticks_per_predict, 4, "Number of cycles taken to process each "
              "prediction");
DEFINE_string(eval_file, "data/eval.bin", "Name of file containing evaluation "
              "data for analysis");

// Training flags.
DEFINE_uint64(num_iter, 1000000, "Number of iterations of training and "
              "validation run for");
DEFINE_uint64(port, 1337, "Port at which the model trainer serves a basic "
              "interface");

namespace {

void LoadReducedMNIST(ImgClfDataset* reduced_mnist, Trace* trace) {
    MNIST mnist;
    mnist.Load(FLAGS_mnist_dir, trace);
    trace->Enter("reduce_mnist_to_14x14");
    mnist.AvgPool({1, 2, 2}, reduced_mnist);
    trace->Exit();
}

void CreateModel(const Dataset& dataset, Model* model, Trace* trace) {
    trace->Enter("create_model");
    auto io = new Adapter;
    auto act_momentum = static_cast<float>(FLAGS_act_momentum);
    auto x_repeats = static_cast<size_t>(FLAGS_x_repeats);
    auto y_repeats = static_cast<size_t>(FLAGS_y_repeats);
    io->Init(act_momentum, x_repeats, dataset.x_size(), y_repeats,
             dataset.y_size());
    auto num_neurons = static_cast<size_t>(FLAGS_num_neurons);
    assert(io->total_size() <= num_neurons);
    auto correlation_momentum = static_cast<float>(FLAGS_correlation_momentum);
    model->Init(io, num_neurons, correlation_momentum);
    trace->Exit();
}

void Run(const Dataset& dataset, Model* model, Trace* trace) {
    trace->Enter("run");
    auto train_split = static_cast<size_t>(FLAGS_train_split);
    auto test_split = static_cast<size_t>(FLAGS_test_split);
    auto ticks_per_train = static_cast<size_t>(FLAGS_ticks_per_train);
    auto ticks_per_predict = static_cast<size_t>(FLAGS_ticks_per_predict);
    Trainer trainer;
    trainer.Init(&dataset, train_split, test_split, model, ticks_per_train,
                 ticks_per_predict, FLAGS_eval_file);
    auto num_iter = static_cast<size_t>(FLAGS_num_iter);
    auto port = static_cast<uint16_t>(FLAGS_port);
printf("about to trainer start\n");
    trainer.Start(num_iter, port);
printf("trainer start returned\n");
    trace->Exit();
}

}  // namespace

int main(int argc, char* argv[]) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    Trace trace;
    trace.Init();

    ImgClfDataset dataset;
    LoadReducedMNIST(&dataset, &trace);

    Model model;
    CreateModel(dataset, &model, &trace);

    Run(dataset, &model, &trace);
}
