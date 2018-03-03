#include <cassert>
#include <chrono>
#include <thread>

#include "base/time/trace.h"

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

    string text;
    trace.Report(&text);
    assert(!text.empty());
}
