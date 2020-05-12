#ifndef ADDRESSABLE_HPP_
#define ADDRESSABLE_HPP_

#include <cstdint>

namespace psx {

  enum class address_width_t {
    byte = 1,
    half = 2,
    word = 4
  };

  class addressable_t {
    const char *name = {};
    bool log_enabled = {};

  public:
    addressable_t(const char *name, bool log_enabled);
    virtual ~addressable_t() {}

    void log(const char *fmt, ...) const;

    virtual uint32_t io_read(address_width_t width, uint32_t address);
    virtual void io_write(address_width_t width, uint32_t address, uint32_t data);
  };
}  // namespace psx

#endif  // ADDRESSABLE_HPP_
