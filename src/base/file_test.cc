#include <cassert>
#include <cstdio>
#include <cstdlib>

#include "base/file.h"

using psyence::base::file::FileSize;
using psyence::base::file::FileToString;
using psyence::base::file::LoadFileTo;

int main() {
    auto filename = "test.bin";
    size_t count = 1024;

    string a;
    for (size_t i = 0; i < count; ++i) {
        a += static_cast<char>(rand() % 256);
    }
    FILE* f = fopen(filename, "w");
    fwrite(a.data(), sizeof(char), count, f);
    fclose(f);

    size_t file_size;
    assert(FileSize(filename, &file_size));
    assert(file_size == count);

    string buf;
    buf.resize(file_size);
    assert(LoadFileTo(filename, file_size,
                      reinterpret_cast<uint8_t*>(&buf[0])));
    assert(buf == a);

    string buf2 = FileToString(filename);
    assert(buf == buf2);

    remove(filename);
}
