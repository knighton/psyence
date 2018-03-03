#include "trace.h"

#include <cassert>
#include <cstdio>

#include "base/str/sprintf.h"
#include "base/time/clock.h"

using psyence::base::str::sprintf::StringAppendF;
using psyence::base::time::clock::NanoClock;

namespace psyence {
namespace base {
namespace time {

void TimedSection::Init(TimedSection* parent, const string& name) {
    name_ = name;
    parent_ = parent;
    children_.clear();
    enter_ns_ = 0;
    exit_ns_ = 0;
}

void TimedSection::Enter(int64_t enter_ns) {
    enter_ns_ = enter_ns;
}

void TimedSection::ReturnedFrom(TimedSection* sub) {
    children_.emplace_back(sub);
}

void TimedSection::Exit(int64_t exit_ns) {
    exit_ns_ = exit_ns;
}

void TimedSection::Report(const DurationPrettyPrinter& pp, size_t num_indents,
                          size_t indent_size, string* text) const {
    auto duration_ns = exit_ns_ - enter_ns_;
    string duration_str;
    pp.PrintNanosec(duration_ns, &duration_str);
    string spacing = string(num_indents * indent_size, ' ');
    StringAppendF(text, "%s%s: %s\n", spacing.data(), name_.data(),
                  duration_str.data());
    for (auto& child : children_) {
        child->Report(pp, num_indents + 1, indent_size, text);
    }
}

void Trace::Free() {
    if (root_) {
        delete root_;
    }
}

Trace::~Trace() {
    Free();
}

void Trace::Init() {
    Free();
    root_ = new TimedSection;
    root_->Init(nullptr, "");
    head_ = root_;
}

void Trace::Enter(const string& name) {
    auto sub = new TimedSection;
    sub->Init(head_, name);
    head_->ReturnedFrom(sub);
    head_ = sub;
    head_->Enter(NanoClock());
}

void Trace::Exit() {
    head_->Exit(NanoClock());
    assert(head_->parent());
    head_ = head_->parent();
}

void Trace::Report(const DurationPrettyPrinter& pp, string* text) const {
    assert(head_ == root_);
    for (auto& child : root_->children()) {
        child->Report(pp, 0, 2, text);
    }
}

ScopeTracer::ScopeTracer(const string& name, Trace* trace) {
    trace_ = trace;
    trace_->Enter(name);
}

ScopeTracer::~ScopeTracer() {
    trace_->Exit();
}

}  // namespace time
}  // namespace base
}  // namespace psyence
