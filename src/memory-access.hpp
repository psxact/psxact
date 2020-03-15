#ifndef MEMORY_ACCESS_HPP_
#define MEMORY_ACCESS_HPP_

#include <cstdint>

namespace psx {

class memory_access_t {
 public:
  virtual ~memory_access_t() {}

  virtual uint8_t read_byte(uint32_t address) = 0;
  virtual uint16_t read_half(uint32_t address) = 0;
  virtual uint32_t read_word(uint32_t address) = 0;

  virtual void write_byte(uint32_t address, uint8_t data) = 0;
  virtual void write_half(uint32_t address, uint16_t data) = 0;
  virtual void write_word(uint32_t address, uint32_t data) = 0;
};

}  // namespace psx

#endif  // MEMORY_ACCESS_HPP_
