#ifndef __PSXACT_UTILITY_HPP__
#define __PSXACT_UTILITY_HPP__

#include <stdio.h>
#include <stdint.h>

#define likely(n) __builtin_expect(!!(n), 1)
#define unlikely(n) __builtin_expect(!!(n), 0)

namespace utility {
  constexpr bool log_cdrom = 0;
  constexpr bool log_cpu   = 0;
  constexpr bool log_dma   = 0;
  constexpr bool log_gpu   = 0;
  constexpr bool log_input = 0;
  constexpr bool log_spu   = 0;
  constexpr bool log_timer = 0;

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

  template<int32_t min, int32_t max>
  inline int32_t sclamp(int32_t value) {
    if (value < min) {
      return min;
    }

    if (value > max) {
      return max;
    }

    return value;
  }

  template<int bits>
  inline int32_t uclamp(int32_t value) {
    enum {
      min = 0,
      max = (1 << bits) - 1
    };

    if (value < min) return min;
    if (value > max) return max;
    return value;
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

  inline uint8_t dec_to_bcd(uint8_t value) {
    return uint8_t(((value / 10) * 16) + (value % 10));
  }

  inline uint8_t bcd_to_dec(uint8_t value) {
    return uint8_t(((value / 16) * 10) + (value % 16));
  }

  template<int bits>
  inline int clz(uint32_t value) {
    constexpr int mask = 1 << (bits - 1);

    uint32_t count = 0;

    while (!(value & mask) && count < bits) {
      value = value << 1;
      count = count + 1;
    }

    return count;
  }
}

#endif // __PSXACT_UTILITY_HPP__
