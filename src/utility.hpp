#ifndef __PSXACT_UTILITY_HPP__
#define __PSXACT_UTILITY_HPP__

#include <stdio.h>
#include <stdint.h>

namespace utility {
  template<int bits>
  struct memory_t {
    static constexpr int mask = (1 << bits) - 1;
    static constexpr int size = (1 << bits);

    union {
      uint8_t  b[size];
      uint16_t h[size / 2];
      uint32_t w[size / 4];
    };
  };

  template<int bits>
  void read_all_bytes(const char *filename, memory_t<bits> &memory) {
    if (FILE* file = fopen(filename, "rb+")) {
      fread(memory.b, 1, memory.size, file);
      fclose(file);
    }
  }

  template<int bits>
  uint32_t read_byte(const memory_t<bits> &memory, uint32_t address) {
    return memory.b[(address & memory.mask) / 1];
  }

  template<int bits>
  uint32_t read_half(const memory_t<bits> &memory, uint32_t address) {
    return memory.h[(address & memory.mask) / 2];
  }

  template<int bits>
  uint32_t read_word(const memory_t<bits> &memory, uint32_t address) {
    return memory.w[(address & memory.mask) / 4];
  }

  template<int bits>
  void write_byte(memory_t<bits> &memory, uint32_t address, uint32_t data) {
    memory.b[(address & memory.mask) / 1] = uint8_t(data);
  }

  template<int bits>
  void write_half(memory_t<bits> &memory, uint32_t address, uint32_t data) {
    memory.h[(address & memory.mask) / 2] = uint16_t(data);
  }

  template<int bits>
  void write_word(memory_t<bits> &memory, uint32_t address, uint32_t data) {
    memory.w[(address & memory.mask) / 4] = data;
  }

  template<int bits>
  inline uint32_t sclip(uint32_t value) {
    enum { mask = (1 << bits) - 1 };
    enum { sign = 1 << (bits - 1) };

    return ((value & mask) ^ sign) - sign;
  }

  template<int bits>
  inline uint32_t uclip(uint32_t value) {
    enum { mask = (1 << bits) - 1 };
    enum { sign = 0 };

    return ((value & mask) ^ sign) - sign;
  }

  template<unsigned min, unsigned max>
  inline bool between(unsigned value) {
    return (value & ~(min ^ max)) == min;
  }

  template<unsigned value>
  constexpr unsigned kib() {
    return 1024 * value;
  }

  template<unsigned value>
  constexpr unsigned mib() {
    return 1024 * kib<value>();
  }
}

#endif // __PSXACT_UTILITY_HPP__
