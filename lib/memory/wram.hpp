#ifndef WRAM_HPP_
#define WRAM_HPP_

#include "memory.hpp"

namespace psx::memory {

  class wram_t : public memory_t< mib(2) > {
  public:
    wram_t();

    uint32_t io_read(address_width_t width, uint32_t address) override;
    void io_write(address_width_t width, uint32_t address, uint32_t data) override;
  };
}

#endif
