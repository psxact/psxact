#ifndef ADDRESSABLE_HPP_
#define ADDRESSABLE_HPP_

#include <cstdint>

#include "util/logger.hpp"
#include "opts.hpp"

namespace psx {

  enum class address_width {
    byte = 1,
    half = 2,
    word = 4
  };

  class addressable {
    util::logger logger;

  public:
    addressable(opts &o, component c);
    virtual ~addressable() {}

    virtual uint32_t io_read(address_width width, uint32_t address);
    virtual void io_write(address_width width, uint32_t address, uint32_t data);
  };
}

#endif
