#ifndef WRAM_HPP_
#define WRAM_HPP_

#include "memory/memory-base.hpp"

namespace psx::memory {

  class wram : public memory_base<mib(2)> {
  public:
    wram(opts &o);

    uint32_t io_read(address_width width, uint32_t address) override;
    void io_write(address_width width, uint32_t address, uint32_t data) override;
  };
}

#endif
