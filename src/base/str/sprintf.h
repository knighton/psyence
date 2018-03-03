#include <string>

using std::string;

namespace psyence {
namespace base {
namespace str {
namespace sprintf {

void SStringPrintf(string* output, const char* format, ...);
void StringAppendF(string* output, const char* format, ...);
string StringPrintf(const char* format, ...);

}  // namespace sprintf
}  // namespace str
}  // namespace base
}  // namespace psyence
