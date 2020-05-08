#ifndef MEMORY_CONTROL_HPP_
#define MEMORY_CONTROL_HPP_

#include "addressable.hpp"

namespace psx {

  class memory_control_t : public addressable_t {
  public:
    memory_control_t();
    ~memory_control_t() {}

    uint32_t io_read(address_width_t width, uint32_t address);
    void io_write(address_width_t width, uint32_t address, uint32_t data);
  };
}  // namespace psx

#endif  // MEMORY_CONTROL_HPP_
