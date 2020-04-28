#include "addressable.hpp"

#include <cstdio>
#include <cstdarg>

using namespace psx;

addressable_t::addressable_t(const char *name, bool log_enabled)
  : name(name)
  , log_enabled(log_enabled) {
}

void addressable_t::log(const char *format, ...) const {
  va_list arg;

  if (log_enabled) {
    printf("[%s] ", name);

    va_start(arg, format);
    vprintf(format, arg);
    va_end(arg);

    printf("\n");
  }
}

uint8_t addressable_t::io_read_byte(uint32_t address) {
  log("io_read_byte(0x%08x)", address);
  return 0;
}

uint16_t addressable_t::io_read_half(uint32_t address) {
  log("io_read_half(0x%08x)", address);
  return 0;
}

uint32_t addressable_t::io_read_word(uint32_t address) {
  log("io_read_word(0x%08x)", address);
  return 0;
}

void addressable_t::io_write_byte(uint32_t address, uint8_t data) {
  log("io_write_byte(0x%08x, 0x%08x)", address, data);
}

void addressable_t::io_write_half(uint32_t address, uint16_t data) {
  log("io_write_half(0x%08x, 0x%08x)", address, data);
}

void addressable_t::io_write_word(uint32_t address, uint32_t data) {
  log("io_write_word(0x%08x, 0x%08x)", address, data);
}
