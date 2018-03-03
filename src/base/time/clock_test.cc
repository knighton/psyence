#include <cassert>
#include <cstdio>

#include "base/time/clock.h"

using psyence::base::time::clock::ClockNanosec;
using psyence::base::time::clock::ClockSec;

int main() {
    // Warmup.
    for (int i = 0; i < 1000; ++i) {
    }

    // Time it.
    auto t_begin = ClockSec();
    for (int i = 0; i < 100; ++i) {
        auto t0 = ClockNanosec();
        for (int j = 0; j < 1000; ++j) {
        }
        auto t = ClockNanosec() - t0;
        assert(0 < t && t < 500);
    }
    auto tt = ClockSec() - t_begin;
    assert(0 < tt && tt < 1);
}
