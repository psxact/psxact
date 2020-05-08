#ifndef MEMORY_HPP_
#define MEMORY_HPP_

#include <cstdint>
#include <cstring>
#include <cstdio>
#include "addressable.hpp"

namespace psx {

constexpr uint32_t kib(uint32_t x) { return 1024 * x; }
constexpr uint32_t mib(uint32_t x) { return 1024 * kib(x); }
constexpr uint32_t gib(uint32_t x) { return 1024 * mib(x); }

template<uint32_t kSize>
struct memory_t : public addressable_t {
  static constexpr int kMask = kSize - 1;

  union {
    uint8_t  b[kSize];
    uint16_t h[kSize / 2];
    uint32_t w[kSize / 4];
  };

  explicit memory_t(const char *name)
    : addressable_t(name, false)
    , b() {
  }

  void *get_pointer(uint32_t address) {
    return &b[address];
  }

  uint32_t io_read(address_width_t width, uint32_t address) {
    switch (width) {
      case address_width_t::byte: return b[(address & kMask) / 1];
      case address_width_t::half: return h[(address & kMask) / 2];
      case address_width_t::word: return w[(address & kMask) / 4];
    }

    return addressable_t::io_read(width, address);
  }

  void io_write(address_width_t width, uint32_t address, uint32_t data) {
    switch (width) {
      case address_width_t::byte: b[(address & kMask) / 1] = uint8_t(data); return;
      case address_width_t::half: h[(address & kMask) / 2] = uint16_t(data); return;
      case address_width_t::word: w[(address & kMask) / 4] = data; return;
    }

    return addressable_t::io_write(width, address, data);
  }

  bool load_blob(const char *filename) {
    if (FILE *file = fopen(filename, "rb+")) {
      fread(b, sizeof(uint8_t), kSize, file);
      fclose(file);
      return true;
    }
    else {
      log("unable to load '%s'", filename);

      return false;
    }
  }
};

}  // namespace psx

#endif  // MEMORY_HPP_
