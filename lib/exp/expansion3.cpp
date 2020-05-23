#include "exp/expansion3.hpp"

#include "timing.hpp"

using namespace psx::exp;

expansion3::expansion3()
  : addressable("exp3", false) {
}

static void add_cpu_time(psx::address_width width) {
  switch (width) {
    case psx::address_width::byte: psx::timing::add_cpu_time(7); break;
    case psx::address_width::half: psx::timing::add_cpu_time(7); break;
    case psx::address_width::word: psx::timing::add_cpu_time(11); break;
  }
}

uint32_t expansion3::io_read(address_width width, uint32_t address) {
  add_cpu_time(width);

  return addressable::io_read(width, address);
}

void expansion3::io_write(address_width width, uint32_t address, uint32_t data) {
  add_cpu_time(width);

  return addressable::io_write(width, address, data);
}
