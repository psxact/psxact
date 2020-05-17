#ifndef BIOS_HPP_
#define BIOS_HPP_

#include "memory.hpp"

namespace psx::memory {

  using bios_base_t = memory_t<kib(512)>;

  class bios_t : public bios_base_t {
  public:
    bios_t();

    uint32_t io_read(address_width_t width, uint32_t address) override;
    void io_write(address_width_t width, uint32_t address, uint32_t data) override;
  };
}

#endif
