#ifndef EXP_EXPANSION1_HPP_
#define EXP_EXPANSION1_HPP_

#include "addressable.hpp"

namespace psx::exp {

  class expansion1_t final : public addressable_t {
  public:
    expansion1_t();

    uint32_t io_read(address_width_t width, uint32_t address);
  };
}  // namespace psx::exp

#endif  // EXP_EXPANSION1_HPP_
