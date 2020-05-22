#ifndef BIOS_HPP_
#define BIOS_HPP_

#include "memory/memory-base.hpp"

namespace psx::memory {

  using bios_base = memory_base<kib(512)>;

  class bios : public bios_base {
  public:
    bios();

    uint32_t io_read(address_width width, uint32_t address) override;
    void io_write(address_width width, uint32_t address, uint32_t data) override;
  };
}

#endif
