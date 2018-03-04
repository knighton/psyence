#include <cassert>

#include "base/collection/json.h"

using psyence::base::collection::json;

int main() {
    json x;
    x["cat"] = {1, 2, 3};
    auto s = x.dump();
    assert(s == "{\"cat\":[1,2,3]}");
}
