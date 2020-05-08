#include "addressable.hpp"

#include <cassert>
#include <cstdio>
#include <cstdarg>

using namespace psx;

addressable_t::addressable_t(const char *name, bool log_enabled)
  : name(name)
  , log_enabled(log_enabled) {
}

void addressable_t::log(const char *format, ...) const {
  if (log_enabled) {
    printf("[%s] ", name);

    va_list arg;
    va_start(arg, format);
    vprintf(format, arg);
    va_end(arg);

    printf("\n");
  }
}

uint32_t addressable_t::io_read(address_width_t width, uint32_t address) {
  log_enabled = true;
  log("io_read(%d, 0x%08x)", width, address);

  assert(0 && "Unhandled I/O read.");
}

void addressable_t::io_write(address_width_t width, uint32_t address, uint32_t data) {
  log_enabled = true;
  log("io_write(%d, 0x%08x, 0x%08x)", width, address, data);

  assert(0 && "Unhandled I/O write.");
}
