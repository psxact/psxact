#ifndef CPU_BIOS_DECODER_HPP_
#define CPU_BIOS_DECODER_HPP_

#include <cstdio>
#include <string>
#include "addressable.hpp"

namespace psx::bios {

class decoder_t final {
 private:
  addressable_t &memory;

 public:
  explicit decoder_t(addressable_t &memory);

  void decode_a(uint32_t pc, uint32_t function, uint32_t *args);

  void decode_b(uint32_t pc, uint32_t function, uint32_t *args);

  void decode_c(uint32_t pc, uint32_t function, uint32_t *args);

 private:
  uint8_t read_byte(uint32_t address);

  std::string decode_string(uint32_t arg);

  std::string decode_timecode(uint32_t arg);
};

}  // namespace psx::bios

#endif  // CPU_BIOS_DECODER_HPP_
