#ifndef UTIL_BLOB_HPP_
#define UTIL_BLOB_HPP_

#include <cstdint>
#include <cstring>
#include <cstdio>

namespace psx::util {

class blob_t {
  uint8_t *buffer;

public:
  blob_t(int size) {
    buffer = new uint8_t[size];
  }

  ~blob_t() {
    delete [] buffer;
  }

  uint32_t read_byte(uint32_t address) {
    return buffer[address];
  }

  uint32_t read_half(uint32_t address) {
    uint32_t lower = read_byte(address & ~1);
    uint32_t upper = read_byte(address |  1);
    return ((upper << 8) | lower);
  }

  uint32_t read_word(uint32_t address) {
    uint32_t lower = read_half(address & ~2);
    uint32_t upper = read_half(address |  2);
    return ((upper << 16) | lower);
  }

  void write_byte(uint32_t address, uint32_t data) {
    buffer[address] = uint8_t(data);
  }

  void write_half(uint32_t address, uint32_t data) {
    write_byte(address & ~1, data);
    write_byte(address |  1, data >> 8);
  }

  void write_word(uint32_t address, uint32_t data) {
    write_half(address & ~2, data);
    write_half(address |  2, data >> 16);
  }

  static blob_t *from_file(const char *filename) {
    if (FILE* file = fopen(filename, "rb+")) {
      fseek(file, 0, SEEK_END);
      int size = ftell(file);
      fseek(file, 0, SEEK_SET);

      blob_t *blob = new blob_t(size);

      fread(blob->buffer, sizeof(uint8_t), size, file);
      fclose(file);
      return blob;
    }
    else {
      printf("unable to load '%s'\n", filename);
      return nullptr;
    }
  }
};

}  // namespace psx::util

#endif  // UTIL_BLOB_HPP_
