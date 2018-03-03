#include <cassert>
#include <cstdio>

#include "base/time/clock.h"

using psyence::base::time::clock::NanoClock;
using psyence::base::time::clock::Clock;

int main() {
    // Warmup.
    for (int i = 0; i < 1000; ++i) {
    }

    // Time it.
    auto t_begin = Clock();
    for (int i = 0; i < 100; ++i) {
        auto t0 = NanoClock();
        for (int j = 0; j < 1000; ++j) {
        }
        auto t = NanoClock() - t0;
        assert(0 < t && t < 500);
    }
    auto tt = Clock() - t_begin;
    assert(0 < tt && tt < 1);
}
