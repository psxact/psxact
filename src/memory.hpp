#ifndef __PSXACT_MEMORY_HPP__
#define __PSXACT_MEMORY_HPP__

#include <cstdint>

template<int bits>
struct memory_t {
  static const int mask = (1 << bits) - 1;
  static const int size = (1 << bits);

  union {
    uint8_t  b[size];
    uint16_t h[size / 2];
    uint32_t w[size / 4];
  };

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
};

#endif // __PSXACT_MEMORY_HPP__
