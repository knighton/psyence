#include <cassert>
#include <tuple>
#include <vector>

#include "base/time/duration_pretty_printer.h"

using psyence::base::time::AsMilliseconds;
using psyence::base::time::AsTightestSubseconds;
using std::get;
using std::tuple;
using std::vector;

int main() {
    vector<tuple<int64_t, string, string>> tuples{
        {1739394125, "1739.394 ms",   "1.739 sec"},
        { 739394125,  "739.394 ms", "739.394 ms"},
        {  39394125,   "39.394 ms",  "39.394 ms"},
        {   9394125,    "9.394 ms",   "9.394 ms"},
        {    394125,    "0.394 ms", "394.125 us"},
        {     94125,    "0.094 ms",  "94.125 us"},
        {      4125,    "0.004 ms",   "4.125 us"},
        {       125,    "0.000 ms", "125.0 ns"},
        {        25,    "0.000 ms",  "25.0 ns"},
        {         5,    "0.000 ms",   "5.0 ns"},
    };

    auto as_ms = AsMilliseconds::New();
    auto as_tight = AsTightestSubseconds::New();

    for (auto& it : tuples) {
        auto& duration_ns = get<0>(it);
        auto& want_ms_str = get<1>(it);
        auto& want_tight_str = get<2>(it);
        string got_ms_str;
        as_ms->PrintNanosec(duration_ns, &got_ms_str);
        string got_tight_str;
        as_tight->PrintNanosec(duration_ns, &got_tight_str);
        assert(got_ms_str == want_ms_str);
        assert(got_tight_str == want_tight_str);
    }

    delete as_ms;
    delete as_tight;
}
