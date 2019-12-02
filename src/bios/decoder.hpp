// Copyright 2018 psxact

#ifndef CPU_BIOS_DECODER_HPP_
#define CPU_BIOS_DECODER_HPP_

#include <cstdio>
#include <string>
#include "memory-access.hpp"

namespace psx {
namespace bios {

class decoder_t {
 private:
  memory_access_t *memory;

 public:
  explicit decoder_t(memory_access_t *memory);

  void decode_a(uint32_t pc, uint32_t function, uint32_t *args);

  void decode_b(uint32_t pc, uint32_t function, uint32_t *args);

  void decode_c(uint32_t pc, uint32_t function, uint32_t *args);

 private:
  uint8_t read_byte(uint32_t address);

  std::string decode_string(uint32_t arg);

  std::string decode_timecode(uint32_t arg);
};

}  // namespace bios
}  // namespace psx

#endif  // CPU_BIOS_DECODER_HPP_