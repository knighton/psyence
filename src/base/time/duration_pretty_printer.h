#pragma once

// Time duration formatters.
//
// These are used for logging/pretty printing time durations.
//
// +- DurationPrettyPrinter [abstract]
//    +- StandardDurationPP [abstract]
//       +- FixedUnitDurationPP (unit_name, unit_precision, total_precision)
//       |  +- AsSeconds (unit_name, digits_after_decimal)
//       |  +- AsMilliseconds (unit_name, digits_after_decimal)
//       |  +- AsMicroseconds (unit_name, digits_after_decimal)
//       |  +- AsNanoseconds (unit_name)
//       |
//       +- DynamicUnitDurationPP (units, digits_after_decimal)
//          +- AsTightestSubseconds (units, digits_after_decimal)

#include <string>
#include <utility>
#include <vector>

using std::pair;
using std::string;
using std::vector;

namespace psyence {
namespace base {
namespace time {

typedef uint8_t Digits;

// Time duration pretty printer abstract base class.
//
// Print methods hand off the duration as int64 nanosec to PrintInternal(),
// which subclasses implement.
class DurationPrettyPrinter {
  public:
    // Dummy constructor/destructor.
    DurationPrettyPrinter();
    virtual ~DurationPrettyPrinter();

    // Print a time duration given in seconds.
    inline void PrintSec(int64_t sec, string* as_str) const {
        PrintInternal(sec * NS_PER_SEC, as_str);
    }
    inline void PrintSec(double sec, string* as_str) const {
        auto ns = static_cast<int64_t>(sec * NS_PER_SEC);
        PrintInternal(ns, as_str);
    }

    // Print a time duration given in milliseconds.
    inline void PrintMillisec(int64_t ms, string* as_str) const {
        PrintInternal(ms * NS_PER_MS, as_str);
    }
    inline void PrintMillisec(double ms, string* as_str) const {
        auto ns = static_cast<int64_t>(ms * NS_PER_MS);
        PrintInternal(ns, as_str);
    }

    // Print a time duration given in microseconds.
    inline void PrintMicrosec(int64_t us, string* as_str) const {
        PrintInternal(us * NS_PER_US, as_str);
    }
    inline void PrintMicrosec(double us, string* as_str) const {
        auto ns = static_cast<int64_t>(us * NS_PER_US);
        PrintInternal(ns, as_str);
    }

    // Print a time duration given in nanoseconds.
    inline void PrintNanosec(int64_t ns, string* as_str) const {
        PrintInternal(ns, as_str);
    }
    inline void PrintNanosec(double ns, string* as_str) const {
        PrintInternal(static_cast<int64_t>(ns), as_str);
    }

  protected:
    // Nanoseconds per unit.
    static const int32_t NS_PER_SEC{1000000000};
    static const int32_t NS_PER_MS{1000000};
    static const int32_t NS_PER_US{1000};

    // Does the work.  Implemented by subclasses.
    virtual void PrintInternal(int64_t ns, string* as_str) const = 0;
};

// Abstract base class for normal-looking time duration formatters.
//
// Prints times in an exact pattern like "<digits>.<digits> <unit>".
class StandardDurationPP : public DurationPrettyPrinter {
  protected:
    StandardDurationPP();
    virtual ~StandardDurationPP();

    // Nanoseconds per decimal place.
    static constexpr int32_t NS_PER_DECIMAL_PLACE[] = {
        1000000000,  // Seconds.
        100000000,
        10000000,
        1000000,  // Mililseconds.
        100000,
        10000,
        1000,  // Microseconds.
        100,
        10,
        1,  // Nanoseconds.
    };

    // Does the actual formatting work, with all required parameters.
    virtual void StandardPrint(Digits unit_precision, Digits total_precision,
                               const string& unit_name, int64_t duration_ns,
                               string* as_str) const;
};

// Displays times in a pre-specified unit.
class FixedUnitDurationPP : public StandardDurationPP {
  public:
    // Initialize with the name of the unit and how many digits of precision.
    //
    // unit_name:
    // * Name of the unit to display after the time duration number.  Can be
    //   anything you want.
    // * Leave empty to not display a unit name.
    //
    // unit_precision:
    // * This is the number of digits after the decimal compared to seconds.
    // * Must be 0 <= unit_precision <= 9.
    // * Example values: 0 = seconds, 3 = milliseconds, 6 = microseconds, etc.
    //
    // total_precision:
    // * Precision including any additional digits after the decimal when
    //   displaying the unit.
    // * 0 <= unit_precision <= total_precision <= 9 (cannot pass 9, because our
    //   clock resolution is nanoseconds).
    FixedUnitDurationPP(const string& unit_name, Digits unit_precision,
                        Digits total_precision);

    // Apply the formatting to the time duration.
    virtual void PrintInternal(int64_t duration_ns, string* as_str) const;

  protected:
    // Name of the unit to display (set to nullptr to not use).
    string unit_name_;

    // Which time unit to use, stored as digits after the decimal.
    //
    // Example values (must be 0 <= unit_precision_ <= 9):
    // * 0 (seconds)
    // * 3 (milliseconds)
    // * 6 (microseconds)
    // * 9 (nanoseconds)
    Digits unit_precision_;

    // Total precision shown including the digits after the decimal.
    //
    // Must be unit_precision_ <= total_precision_ <= 9.
    Digits total_precision_;
};

// Print all times to seconds.
class AsSeconds : public FixedUnitDurationPP {
  public:
    // Create an AsSeconds on the heap with reasonable defaults.
    static AsSeconds* New();

    // You must provide the unit name ("sec", "seconds", etc.) and number of
    // digits after the decimal.
    AsSeconds(const string& unit_name, Digits digits_after_decimal);
    virtual ~AsSeconds();
};

// Print all times to milliseconds.
class AsMilliseconds : public FixedUnitDurationPP {
  public:
    // Create an AsMilliseconds on the heap with reasonable defaults.
    static AsMilliseconds* New();

    // You must provide the unit name ("ms", "milliseconds", etc.) and number of
    // digits after the decimal.
    AsMilliseconds(const string& unit_name, Digits digits_after_decimal);
    virtual ~AsMilliseconds();
};

// Print all times to microseconds.
class AsMicroseconds : public FixedUnitDurationPP {
  public:
    // Create an AsMicroseconds on the heap with reasonable defaults.
    static AsMicroseconds* New();

    // You must provide the unit name ("us", "microseconds", etc.) and number of
    // digits after the decimal.
    AsMicroseconds(const string& unit_name, Digits digits_after_decimal);
    virtual ~AsMicroseconds();
};

// Print all times to nanoseconds.
class AsNanoseconds : public FixedUnitDurationPP {
  public:
    // Create an AsNanoseconds on the heap with reasonable defaults.
    static AsNanoseconds* New();

    // You must provide the unit name ("ns", "nanoseconds", etc.).  No digits
    // after the decimal because nanosec is maximum precision.
    AsNanoseconds(const string& unit_name);
    virtual ~AsNanoseconds();
};

// Prints durations in units that may vary at runtime.
class DynamicUnitDurationPP : public StandardDurationPP {
  public:
    // Initialize with the list of units and number of extra digits.
    //
    // Units are pairs of (unit name, its digits of precision).
    DynamicUnitDurationPP(const vector<pair<string, Digits>>& units,
                          Digits digits_after_decimal);
    virtual ~DynamicUnitDurationPP();

  protected:
    // List of respective unit names and unit precisions.
    //
    // Unit names are arbitrary strings.
    //
    // Unit precisions are strictly monotonically increasing and between 0 and 9
    // inclusive.
    vector<string> unit_names_;
    vector<Digits> unit_precisions_;

    // The number of digits to show after the decimal in whatever dynamically
    // selected unit.
    Digits digits_after_decimal_;
};

// Prints durations in the unit that fits the number the tightest.
//
// That is, it selects the unit whose resulting duration value is the smallest
// but greater than or equal to one.
class AsTightestSubseconds : public DynamicUnitDurationPP {
  public:
    // Create a AsTightestSubseconds on the heap with reasonable defaults.
    static AsTightestSubseconds* New();

    // Initialize with the list of units and number of extra digits.
    //
    // Units are pairs of (unit name, its digits of precision).
    AsTightestSubseconds(const vector<pair<string, Digits>>& units,
                         Digits digits_after_decimal);
    void PrintInternal(int64_t duration_ns, string* as_str) const;
};

}  // namespace time
}  // namespace base
}  // namespace psyence
