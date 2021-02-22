#ifndef ADDRESSABLE_HPP_
#define ADDRESSABLE_HPP_

#include <cstdint>

#include "opts.hpp"

namespace psx {

  enum class address_width {
    byte = 1,
    half = 2,
    word = 4
  };

  class addressable {
    const char *name = {};
    bool log_enabled = {};

  public:
    addressable(opts &o, component c);
    virtual ~addressable() {}

    void log(const char *fmt, ...) const;

    virtual uint32_t io_read(address_width width, uint32_t address);
    virtual void io_write(address_width width, uint32_t address, uint32_t data);
  };
}

#endif
