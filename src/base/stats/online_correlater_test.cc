#include <cassert>
#include <cstdio>
#include <cstdlib>

#include "base/stats/online_correlater.h"
#include "base/stats/summary.h"

using psyence::base::stats::OnlineCorrelater;
using psyence::base::stats::Summary;

int main() {
    // Test parameters.
    size_t num_steps = 10000;
    size_t num_variables = 100;
    float momentum = 0.99f;
    size_t num_pcts = 0;

    // Setup.
    OnlineCorrelater stats;
    stats.Init(num_variables, momentum);

    // Do a bunch of rounds of updating the online correlater with samples from
    // a random uniform distribution from -1 to +1.  This way we'll know what
    // output distribution to expect.
    float* ff = new float[num_variables];
    for (size_t i = 0; i < num_steps; ++i) {
        for (size_t j = 0; j < num_variables; ++j) {
            auto f = static_cast<float>(rand()) / RAND_MAX;
            ff[j] = f * 2 - 1;
        }
        stats.Update(ff);
    }

    // Means of the variables should cluster tightly around zero.
    {
        Summary x;
        x.InitFromData(num_variables, stats.means(), num_pcts);
        assert(-0.05 < x.mean() && x.mean() < 0.05); 
        assert(0 < x.std() && x.std() < 0.05);
    }

    // Because it's uniform from -1 to +5, standard deviations should cluster
    // tightly around 0.5.
    {
        Summary x;
        x.InitFromData(num_variables, stats.stds(), num_pcts);
        assert(0.45 < x.mean() && x.mean() < 0.55); 
        assert(0 < x.std() && x.std() < 0.05);
    }

    // Covariance matrix values should be tightly clustered around zero (not
    // correlated).
    {
        Summary x;
        x.InitFromData(num_variables * num_variables, stats.cov(), num_pcts);
        assert(0 < x.mean() && x.mean() < 0.05);
        assert(0 < x.std() && x.std() < 0.05); 
    }

    // Pearson correlation coefficients should also cluster around zero.
    {
        Summary x;
        x.InitFromData(num_variables * num_variables, stats.cor(), num_pcts);
        assert(0 < x.mean() && x.mean() < 0.05);
        assert(0.1 < x.std() && x.std() < 0.2); 
    }

    delete [] ff;
}
