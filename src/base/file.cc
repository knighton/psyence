#include "file.h"

#include <cassert>
#include <cstdio>
#include <sys/stat.h>

namespace psyence {
namespace base {
namespace file {

bool FileSize(const char* filename, size_t *num_bytes) {
    struct stat st;
    if (stat(filename, &st)) {
        return false;
    }

    *num_bytes = static_cast<size_t>(st.st_size);
    return true;
}

bool FileToBuffer(const char* filename, size_t num_bytes, uint8_t* buf) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        return false;
    }

    if (fread(buf, sizeof(char), num_bytes, f) != num_bytes) {
        fclose(f);
        return false;
    }

    fclose(f);
    return true;
}

string FileToString(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        assert(false);
    }

    if (fseek(f, 0, SEEK_END)) {
        fclose(f);
        assert(false);
    }

    auto num_bytes = static_cast<size_t>(ftell(f));
    rewind(f);
    string s;
    s.resize(num_bytes);
    assert(fread(&s[0], sizeof(char), num_bytes, f) == num_bytes);
    fclose(f);
    return s;
}

}  // namespace file
}  // namespace base
}  // namespace psyence
