#include <cassert>

#include "ext/json.h"

int main() {
    json x;
    x["cat"] = {1, 2, 3};
    auto s = x.dump();
    assert(s == "{\"cat\":[1,2,3]}");
}
