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

void TimedSection::Report(size_t num_indents, size_t indent_size,
                          string* text) const {
    auto ns = exit_ns_ - enter_ns_;
    string spacing = string(num_indents * indent_size, ' ');
    StringAppendF(text, "%s%s: %ld ns\n", spacing.data(), name_.data(), ns);
    for (auto& child : children_) {
        child->Report(num_indents + 1, indent_size, text);
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

void Trace::Report(string* text) const {
    assert(head_ == root_);
    for (auto& child : root_->children()) {
        child->Report(0, 2, text);
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
