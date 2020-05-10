#ifndef EXP_EXPANSION2_HPP_
#define EXP_EXPANSION2_HPP_

#include "addressable.hpp"

namespace psx::exp {

  class expansion2_t final : public addressable_t {
  public:
    expansion2_t();

    uint32_t io_read(address_width_t width, uint32_t address);
    void io_write(address_width_t width, uint32_t address, uint32_t data);
  };
}  // namespace psx::exp

#endif  // EXP_EXPANSION2_HPP_
