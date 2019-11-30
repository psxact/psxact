// Copyright 2018 psxact

#ifndef MEMORY_HPP_
#define MEMORY_HPP_

#include <cstdint>
#include <cstring>
#include <cstdio>
#include "memory-component.hpp"

namespace psx {

constexpr uint32_t kib(uint32_t x) { return 1024 * x; }
constexpr uint32_t mib(uint32_t x) { return 1024 * kib(x); }
constexpr uint32_t gib(uint32_t x) { return 1024 * mib(x); }

template<uint32_t kSize>
struct memory_t : public memory_component_t {
  static constexpr int kMask = kSize - 1;

  union {
    uint8_t  b[kSize];
    uint16_t h[kSize / 2];
    uint32_t w[kSize / 4];
  };

  explicit memory_t(const char *name)
    : memory_component_t(name, false) {

    memset(b, 0, size_t(kSize));
  }

  void *get_pointer(uint32_t address) {
    return &b[address];
  }

  uint8_t io_read_byte(uint32_t address) {
    return b[(address & kMask) / 1];
  }

  uint16_t io_read_half(uint32_t address) {
    return h[(address & kMask) / 2];
  }

  uint32_t io_read_word(uint32_t address) {
    return w[(address & kMask) / 4];
  }

  void io_write_byte(uint32_t address, uint8_t data) {
    b[(address & kMask) / 1] = uint8_t(data);
  }

  void io_write_half(uint32_t address, uint16_t data) {
    h[(address & kMask) / 2] = uint16_t(data);
  }

  void io_write_word(uint32_t address, uint32_t data) {
    w[(address & kMask) / 4] = data;
  }

  bool load_blob(const char *filename) {
    if (FILE* file = fopen(filename, "rb+")) {
      fread(b, sizeof(uint8_t), kSize, file);
      fclose(file);
      return true;
    } else {
      log("unable to load '%s'", filename);

      return false;
    }
  }
};

}  // namespace psx

#endif  // MEMORY_HPP_
