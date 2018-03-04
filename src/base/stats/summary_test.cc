#include "base/floats.h"
#include "base/stats/summary.h"

#include <cassert>

using psyence::base::floats::FloatEqual;
using psyence::base::stats::Summary;

int main() {
    size_t count = 101;
    auto eps = 1e-3f;

    auto x = new float[count];
    for (size_t i = 0; i < count; ++i) {
        x[i] = static_cast<float>(i);
    }
    Summary stats;
    stats.InitFromData(count, x, 10);
    delete [] x;
    assert(stats.count() == count);
    assert(FloatEqual(stats.min(), 0, eps));
    assert(FloatEqual(stats.max(), 100, eps));
    assert(FloatEqual(stats.sum(), 5050, eps));
    assert(FloatEqual(stats.mean(), 50, eps));
    assert(FloatEqual(stats.std(), 29.155f, eps));
}
