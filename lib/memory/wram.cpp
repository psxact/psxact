#include "memory/wram.hpp"

#include "timing.hpp"

using namespace psx::memory;

wram_t::wram_t()
  : memory_t("wram") {
}

uint32_t wram_t::io_read(address_width_t width, uint32_t address) {
  psx::timing::add_cpu_time(6);
  return memory_t::io_read(width, address);
}

void wram_t::io_write(address_width_t width, uint32_t address, uint32_t data) {
  psx::timing::add_cpu_time(6);
  return memory_t::io_write(width, address, data);
}
