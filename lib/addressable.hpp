#ifndef ADDRESSABLE_HPP_
#define ADDRESSABLE_HPP_

#include <cstdint>

namespace psx {

class addressable_t {
  const char *name = {};

  bool log_enabled = {};

 public:
  explicit addressable_t(const char *name, bool log_enabled);

  virtual ~addressable_t() {}

  void log(const char *fmt, ...) const;

  virtual uint8_t io_read_byte(uint32_t address);
  virtual uint16_t io_read_half(uint32_t address);
  virtual uint32_t io_read_word(uint32_t address);

  virtual void io_write_byte(uint32_t address, uint8_t data);
  virtual void io_write_half(uint32_t address, uint16_t data);
  virtual void io_write_word(uint32_t address, uint32_t data);
};

}  // namespace psx

#endif  // ADDRESSABLE_HPP_
