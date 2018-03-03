#include <cassert>
#include <string>

#include "base/str/sprintf.h"

using psyence::base::str::sprintf::SStringPrintf;
using psyence::base::str::sprintf::StringAppendF;
using psyence::base::str::sprintf::StringPrintf;
using std::string;

int main() {
    string s = StringPrintf("test %c %d %s", 'A', 1337, "wtf");
    assert(s == "test A 1337 wtf");

    SStringPrintf(&s, "%s %d", "over", 9000);
    assert(s == "over 9000");

    StringAppendF(&s, "%d %s", 1, "lolcats");
    assert(s == "over 90001 lolcats");
}
