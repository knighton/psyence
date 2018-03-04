#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wweak-vtables"

#include "ext/json/nlohmann_json.h"

#pragma clang diagnostic pop

namespace psyence {
namespace base {
namespace collection {

using json = nlohmann::json;

}  // namespace collection
}  // namespace base
}  // namespace psyence
