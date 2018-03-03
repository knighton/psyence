#include "duration_pretty_printer.h"

#include <cassert>

#include "base/str/sprintf.h"

using psyence::base::str::sprintf::SStringPrintf;

namespace psyence {
namespace base {
namespace time {

DurationPrettyPrinter::DurationPrettyPrinter() {
}

DurationPrettyPrinter::~DurationPrettyPrinter() {
}

StandardDurationPP::StandardDurationPP() :
        DurationPrettyPrinter() {
}

StandardDurationPP::~StandardDurationPP() {
}

void StandardDurationPP::StandardPrint(
        Digits unit_precision, Digits total_precision,
        const string& unit_name, int64_t duration_ns, string* as_str) const {
    // Compute the integer part.
    // * Nanoseconds per the selected unit.
    // * The integer part of the duration in that unit.
    // * The fractional part of the duration in that unit, in nanoseconds.
    auto ns_per_xs = NS_PER_DECIMAL_PLACE[unit_precision];
    auto int_duration_xs = duration_ns / ns_per_xs;
    auto frac_duration_ns = duration_ns % ns_per_xs;

    // Compute the fractional part (if unit_precision < total_precision).
    // * Nanoseconds per the total precision number of digits.
    // * The fractional part of the duration in the total time resolution.
    // * Number of digits of the fractional part.
    auto ns_per_ys = NS_PER_DECIMAL_PLACE[total_precision];
    auto frac_duration_ys = frac_duration_ns / ns_per_ys;
    auto frac_num_digits = total_precision - unit_precision;

    // Print.
    // * Get name of the time unit.
    // * Create the format string.
    // * Insert digits.
    // * Insert unit name if we have it.
    string fmt;
    SStringPrintf(&fmt, "%%ld.%%0%dld", frac_num_digits);
    SStringPrintf(as_str, fmt.data(), int_duration_xs, frac_duration_ys,
                  unit_name.data());
    if (!unit_name.empty()) {
        *as_str += " " + unit_name;
    }
}

FixedUnitDurationPP::FixedUnitDurationPP(
        const string& unit_name, Digits unit_precision,
        Digits total_precision) :
            StandardDurationPP() {
    assert(0 <= unit_precision);
    assert(unit_precision <= total_precision);
    assert(total_precision <= 9);
    unit_name_ = unit_name;
    unit_precision_ = unit_precision;
    total_precision_ = total_precision;
    unit_name_ = unit_name;
}

void FixedUnitDurationPP::PrintInternal(
        int64_t duration_ns, string* as_str) const {
    StandardPrint(unit_precision_, total_precision_, unit_name_, duration_ns,
                  as_str);
}

AsSeconds* AsSeconds::New() {
    return new AsSeconds("sec", 3);
}

AsSeconds::AsSeconds(const string& unit_name, Digits digits_after_decimal) :
        FixedUnitDurationPP(unit_name, 0, digits_after_decimal) {
}

AsSeconds::~AsSeconds() {
}

AsMilliseconds* AsMilliseconds::New() {
    return new AsMilliseconds("ms", 3);
}

AsMilliseconds::AsMilliseconds(const string& unit_name,
                           Digits digits_after_decimal) :
        FixedUnitDurationPP(unit_name, 3, 3 + digits_after_decimal) {
}

AsMilliseconds::~AsMilliseconds() {
}

AsMicroseconds* AsMicroseconds::New() {
    return new AsMicroseconds("us", 3);
}

AsMicroseconds::AsMicroseconds(const string& unit_name,
                           Digits digits_after_decimal) :
        FixedUnitDurationPP(unit_name, 6, 6 + digits_after_decimal) {
}

AsMicroseconds::~AsMicroseconds() {
}

AsNanoseconds* AsNanoseconds::New() {
    return new AsNanoseconds("ns");
}

AsNanoseconds::AsNanoseconds(const string& unit_name) :
        FixedUnitDurationPP(unit_name, 9, 9) {
}

AsNanoseconds::~AsNanoseconds() {
}

DynamicUnitDurationPP::DynamicUnitDurationPP(
        const vector<pair<string, Digits>>& units,
        Digits digits_after_decimal) : StandardDurationPP() {
    for (auto& unit : units) {
        auto& name = unit.first;
        auto& precision = unit.second;
        unit_names_.emplace_back(name);
        if (!unit_precisions_.empty()) {
            auto& last_prec = unit_precisions_[unit_precisions_.size() - 1];
            assert(last_prec < precision);
        }
        unit_precisions_.emplace_back(precision);
    }
    digits_after_decimal_ = digits_after_decimal;
}

DynamicUnitDurationPP::~DynamicUnitDurationPP() {
}

AsTightestSubseconds* AsTightestSubseconds::New() {
    vector<pair<string, Digits>> units = {
        {"sec", 0},
        {"ms", 3},
        {"us", 6},
        {"ns", 9},
    };
    Digits digits_after_decimal = 3;
    return new AsTightestSubseconds(units, digits_after_decimal);
}

AsTightestSubseconds::AsTightestSubseconds(
        const vector<pair<string, Digits>>& units,
        Digits digits_after_decimal) :
            DynamicUnitDurationPP(units, digits_after_decimal) {
}

void AsTightestSubseconds::PrintInternal(
        int64_t duration_ns, string* as_str) const {
    size_t i;
    for (i = 0; i < unit_precisions_.size(); ++i) {
        auto& prec = unit_precisions_[i];
        auto& ns_per_xs = StandardDurationPP::NS_PER_DECIMAL_PLACE[prec];
        if (ns_per_xs < duration_ns) {
            goto found;
        }
    }
    --i;
found:
    auto& unit_precision = unit_precisions_[i];
    Digits total_precision = unit_precision + digits_after_decimal_;
    if (9 < total_precision) {
        total_precision = 9;
    }
    auto& unit_name = unit_names_[i];
    StandardPrint(unit_precision, total_precision, unit_name, duration_ns,
                  as_str);
}

}  // namespace time
}  // namespace base
}  // namespace psyence
