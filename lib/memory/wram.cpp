#include "memory/wram.hpp"

#include "timing.hpp"

using namespace psx::memory;

wram::wram(opts &o)
  : memory_base(o, component::wram) {
}

uint32_t wram::io_read(address_width width, uint32_t address) {
  psx::timing::add_cpu_time(6);
  return memory_base::io_read(width, address);
}

void wram::io_write(address_width width, uint32_t address, uint32_t data) {
  psx::timing::add_cpu_time(6);
  return memory_base::io_write(width, address, data);
}
