#include "memory/bios.hpp"

#include "args.hpp"
#include "timing.hpp"

using namespace psx::memory;

bios_t::bios_t() : bios_base_t("bios") {
  load_blob(args::bios_file_name);

  if (args::bios_patch_skip_intro) {
    io_write(address_width_t::word, 0x6990, 0x00000000); // nop
  }

  if (args::bios_patch_debug_tty) {
    io_write(address_width_t::word, 0x6f0c, 0x34010001); // li $at, 0x1
    io_write(address_width_t::word, 0x6f10, 0x0ff019e1); // jal 0xbfc06784
    io_write(address_width_t::word, 0x6f14, 0xaf81a9c0); // sw $at -0x5460($gp)
  }
}

static void add_cpu_time(psx::address_width_t width) {
  switch (width) {
    case psx::address_width_t::byte: psx::timing::add_cpu_time( 8); break; // 8
    case psx::address_width_t::half: psx::timing::add_cpu_time(14); break; // 8+6
    case psx::address_width_t::word: psx::timing::add_cpu_time(26); break; // 8+6+6+6
  }
}

uint32_t bios_t::io_read(address_width_t width, uint32_t address) {
  add_cpu_time(width);

  return memory_t::io_read(width, address);
}

void bios_t::io_write(address_width_t width, uint32_t address, uint32_t data) {
  add_cpu_time(width);

  return memory_t::io_write(width, address, data);
}
