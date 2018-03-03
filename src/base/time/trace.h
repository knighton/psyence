#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "base/time/duration_pretty_printer.h"

using psyence::base::time::DurationPrettyPrinter;
using std::string;
using std::vector;

namespace psyence {
namespace base {
namespace time {

// A timed section of code.
//
// Is recursive -- contains a list of timed subsections.
class TimedSection {
  public:
    // Accessors.
    const string& name() const { return name_; }
    TimedSection* parent() const { return parent_; }
    const vector<TimedSection*>& children() const { return children_; }
    int64_t enter_ns() const { return enter_ns_; }
    int64_t exit_ns() const { return exit_ns_; }

    // Sequence of steps:
    // * Initialize.
    // * Start timing.
    // * Enter and exit subsections.
    // * Stop timing.
    void Init(TimedSection* parent, const string& name);
    void Enter(int64_t enter_ns);
    void ReturnedFrom(TimedSection* sub);
    void Exit(int64_t exit_ns);

    // Dump recursive timings to string, line by line.
    void Report(const DurationPrettyPrinter& pp, size_t num_indents,
                size_t indent_size, string* text) const;

  private:
    // Human-friendly name of this section.
    string name_;

    // The section that called us, and the sections we called.
    TimedSection* parent_;
    vector<TimedSection*> children_;

    // Timings.
    int64_t enter_ns_;
    int64_t exit_ns_;
};

// A tree of timed sections that the code execution passed through.
//
// Used to collect timing information of important things.
class Trace {
  public:
    // Free memory.
    ~Trace();

    // Sequence of steps:
    // * Initialize.
    // * Push a new subsection onto the call tree.
    // * Pop the current subsection off the call tree.
    void Init();
    void Enter(const string& name);
    void Exit();

    // Dump recursive timings to string, line by line.
    void Report(const DurationPrettyPrinter& pp, string* text) const;

  private:
    // Free memory.
    void Free();

    // The root timed code section.  Its enter_ns_ and exit_ns_ are ignored.
    TimedSection* root_{nullptr};

    // Pointer to the lowest-level section that we are currently in.
    TimedSection* head_{nullptr};
};

// A scoped-based timer that updates a Trace.
//
// Starts timing when created.  Stops timing when destroyed (goes out of scope).
class ScopeTracer {
  public:
    // Enter the timed section.
    ScopeTracer(const string& name, Trace* trace);

    // Exit the timed section.
    ~ScopeTracer();

  private:
    // Where we note timed section enter/exit.
    Trace* trace_;
};

}  // namespace time
}  // namespace base
}  // namespace psyence
