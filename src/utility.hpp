// Copyright 2018 psxact

#ifndef UTILITY_HPP_
#define UTILITY_HPP_

#include <cstdint>
#include <cstdio>

#if defined (__clang__) || defined (__GNUC__)
  #define likely(n) __builtin_expect(!!(n), 1)
  #define unlikely(n) __builtin_expect(!!(n), 0)
#else
  #define likely(n)
  #define unlikely(n)
#endif

#if defined (DEBUG)
  #define logger(n, s, ...) printf("[" n "] " s "\n", ## __VA_ARGS__)
#else
  #define logger(n, s, ...)
#endif

#define log_cpu(s, ...) logger("cpu", s, __VA_ARGS__)
#define log_dma(s, ...) logger("dma", s, __VA_ARGS__)
#define log_gpu(s, ...) logger("gpu", s, __VA_ARGS__)
#define log_spu(s, ...) logger("spu", s, __VA_ARGS__)
#define log_cdrom(s, ...) logger("cdrom", s, __VA_ARGS__)
#define log_input(s, ...) logger("input", s, __VA_ARGS__)
#define log_timer(s, ...) logger("timer", s, __VA_ARGS__)

namespace psx {
namespace utility {

template<int bits>
inline uint32_t sclip(uint32_t value) {
  const int mask = (1 << bits) - 1;
  const int sign = 1 << (bits - 1);

  return ((value & mask) ^ sign) - sign;
}

template<int bits>
inline uint32_t uclip(uint32_t value) {
  const int mask = (1 << bits) - 1;

  return value & mask;
}

inline uint8_t dec_to_bcd(uint8_t value) {
  return uint8_t(((value / 10) * 16) + (value % 10));
}

inline uint8_t bcd_to_dec(uint8_t value) {
  return uint8_t(((value / 16) * 10) + (value % 16));
}

template<int bits>
inline int clz(uint32_t value) {
  const int mask = 1 << (bits - 1);

  uint32_t count = 0;

  while (!(value & mask) && count < bits) {
    value = value << 1;
    count = count + 1;
  }

  return count;
}

}  // namespace utility
}  // namespace psx

#endif  // UTILITY_HPP_
