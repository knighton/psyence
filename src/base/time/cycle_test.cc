#include <cassert>
#include <cstdio>

#include "base/time/cycle.h"

using psyence::base::time::cycle::ClockCycles;

int main() {
    // Warmup.
    for (int i = 0; i < 1000; ++i) {
    }

    // Time it.
    for (int i = 0; i < 100; ++i) {
        auto t0 = ClockCycles();
        for (int j = 0; j < 1000; ++j) {
        }
        auto t = ClockCycles() - t0;
        assert(0 < t && t < 100);
    }
}
