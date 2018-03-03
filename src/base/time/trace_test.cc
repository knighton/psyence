#include <cassert>
#include <chrono>
#include <thread>

#include "base/time/duration_pretty_printer.h"
#include "base/time/trace.h"

using psyence::base::time::AsTightestSubseconds;
using psyence::base::time::ScopeTracer;
using psyence::base::time::Trace;
using namespace std::chrono_literals;

int main() {
    Trace trace;
    trace.Init();

    {
        ScopeTracer st("outer", &trace);
        std::this_thread::sleep_for(200us);

        {
            ScopeTracer inner_one_st("inner_one", &trace);
            std::this_thread::sleep_for(27us);
        }

        {
            ScopeTracer inner_two_st("inner_two", &trace);
            std::this_thread::sleep_for(1337us);
        }
    }

    auto dpp = AsTightestSubseconds::New();
    string text;
    trace.Report(*dpp, &text);
    assert(!text.empty());
    delete dpp;
}
