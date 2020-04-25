#ifndef EXP_EXPANSION2_HPP_
#define EXP_EXPANSION2_HPP_

#include "addressable.hpp"

namespace psx::exp {

class expansion2_t final : public addressable_t {
 public:
  expansion2_t();

  uint8_t io_read_byte(uint32_t address);

  void io_write_byte(uint32_t address, uint8_t data);
};

}  // namespace psx::exp

#endif  // EXP_EXPANSION2_HPP_
