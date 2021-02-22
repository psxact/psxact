#ifndef MEMORY_MEMORY_CONTROL_HPP_
#define MEMORY_MEMORY_CONTROL_HPP_

#include "addressable.hpp"

namespace psx::memory {

  class memory_control : public addressable {
    uint32_t biu;
    uint32_t exp1_base;
    uint32_t exp2_base;
    uint32_t exp1_conf;
    uint32_t exp2_conf;
    uint32_t exp3_conf;
    uint32_t bios_conf;
    uint32_t spu_conf;
    uint32_t cdc_conf;
    uint32_t time_conf;
    uint32_t wram_conf;

  public:
    memory_control(opts &o);
    ~memory_control() {}

    uint32_t io_read(address_width width, uint32_t address) override;
    void io_write(address_width width, uint32_t address, uint32_t data) override;
  };
}

#endif
