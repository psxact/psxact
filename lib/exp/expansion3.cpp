#include "exp/expansion3.hpp"

#include "timing.hpp"

using namespace psx::exp;

expansion3_t::expansion3_t()
  : addressable_t("exp3", false) {
}

static void add_cpu_time(psx::address_width_t width) {
  switch (width) {
    case psx::address_width_t::byte: psx::timing::add_cpu_time(7); break;
    case psx::address_width_t::half: psx::timing::add_cpu_time(7); break;
    case psx::address_width_t::word: psx::timing::add_cpu_time(11); break;
  }
}

uint32_t expansion3_t::io_read(address_width_t width, uint32_t address) {
  add_cpu_time(width);

  return addressable_t::io_read(width, address);
}

void expansion3_t::io_write(address_width_t width, uint32_t address, uint32_t data) {
  add_cpu_time(width);

  return addressable_t::io_write(width, address, data);
}
