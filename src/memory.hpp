#ifndef __psxact_memory_hpp__
#define __psxact_memory_hpp__


#include <cstdint>
#include <cstring>
#include <cstdio>


template<int bits>
struct memory_t {
  static const int mask = (1 << bits) - 1;
  static const int size = (1 << bits);

  union {
    uint8_t  b[size];
    uint16_t h[size / 2];
    uint32_t w[size / 4];
  };

  memory_t() {
    memset(b, 0, size_t(size));
  }

  void *get_pointer(uint32_t address) {
    return &b[address];
  }

  uint32_t read_byte(uint32_t address) {
    return b[(address & mask) / 1];
  }

  uint32_t read_half(uint32_t address) {
    return h[(address & mask) / 2];
  }

  uint32_t read_word(uint32_t address) {
    return w[(address & mask) / 4];
  }

  void write_byte(uint32_t address, uint32_t data) {
    b[(address & mask) / 1] = uint8_t(data);
  }

  void write_half(uint32_t address, uint32_t data) {
    h[(address & mask) / 2] = uint16_t(data);
  }

  void write_word(uint32_t address, uint32_t data) {
    w[(address & mask) / 4] = data;
  }

  bool load_blob(const char *filename) {
    if (FILE* file = fopen(filename, "rb+")) {
      fread(b, sizeof(uint8_t), size, file);
      fclose(file);
      return true;
    }
    else {
      printf("unable to load '%s'\n", filename);

      return false;
    }
  }

};


#endif // __psxact_memory_hpp__
