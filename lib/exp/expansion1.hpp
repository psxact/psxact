#ifndef EXP_EXPANSION1_HPP_
#define EXP_EXPANSION1_HPP_

#include "addressable.hpp"

namespace psx::exp {

  class expansion1_t final : public addressable {
  public:
    expansion1_t();

    uint32_t io_read(address_width width, uint32_t address) override;
    void io_write(address_width width, uint32_t address, uint32_t data) override;
  };
}

#endif
