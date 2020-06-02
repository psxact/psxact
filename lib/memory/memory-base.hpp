#ifndef MEMORY_MEMORY_BASE_HPP_
#define MEMORY_MEMORY_BASE_HPP_

#include <cstdint>
#include <cstring>
#include <cstdio>

#include "util/panic.hpp"
#include "addressable.hpp"

namespace psx {

  constexpr uint32_t kib(uint32_t x) { return 1024 * x; }
  constexpr uint32_t mib(uint32_t x) { return 1024 * kib(x); }
  constexpr uint32_t gib(uint32_t x) { return 1024 * mib(x); }

  template<uint32_t kSize>
  class memory_base : public addressable {
    static constexpr int kMask = kSize - 1;

    union {
      uint8_t  b[kSize];
      uint16_t h[kSize / 2];
      uint32_t w[kSize / 4];
    };

  public:
    explicit memory_base(const char *name)
      : addressable(name, false)
      , b() {
    }

    void *get_pointer(uint32_t address) {
      return &b[address];
    }

    uint32_t io_read(address_width width, uint32_t address) {
      switch (width) {
        case address_width::byte: return b[(address & kMask) / 1];
        case address_width::half: return h[(address & kMask) / 2];
        case address_width::word: return w[(address & kMask) / 4];
      }

      return addressable::io_read(width, address);
    }

    void io_write(address_width width, uint32_t address, uint32_t data) {
      switch (width) {
        case address_width::byte: b[(address & kMask) / 1] = uint8_t(data); return;
        case address_width::half: h[(address & kMask) / 2] = uint16_t(data); return;
        case address_width::word: w[(address & kMask) / 4] = data; return;
      }

      return addressable::io_write(width, address, data);
    }

    void load_blob(FILE *file) {
      fread(b, sizeof(uint8_t), kSize, file);
      fclose(file);
    }
  };
}

#endif
