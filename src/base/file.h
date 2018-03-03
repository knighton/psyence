#pragma once

#include <string>

using std::string;

namespace psyence {
namespace base {
namespace file {

// Get the size of the file in bytes.
//
// Returns true on success, false if the file does not exist.
bool FileSize(const char* filename, size_t *num_bytes);

// Load the file into the given preallocated buffer.
//
// See FileSize() for getting the size.  Specifically reads the first num_bytes
// of the file.  Returns true on success, false if the file does not exist or it
// read anything other than num_bytes bytes.
bool LoadFileTo(const char* filename, size_t num_bytes, uint8_t* buf);

// Load the file into a byte array that it allocates.
//
// Saves size to "num_bytes" and data to "buf", which is allocated with new.
bool LoadFile(const char* filename, size_t* num_bytes, uint8_t** buf);

// Convenience method: load the file into a string that is returned.
//
// Crashes on failure.
string FileToString(const char* filename);

}  // namespace file
}  // namespace base
}  // namespace psyence
