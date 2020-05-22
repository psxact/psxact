#include "memory/bios.hpp"

#include "args.hpp"
#include "timing.hpp"

using namespace psx::memory;

bios::bios() : bios_base("bios") {
  load_blob(args::bios_file_name);

  if (args::bios_patch_skip_intro) {
    io_write(address_width::word, 0x6990, 0x00000000); // nop
  }

  if (args::bios_patch_debug_tty) {
    io_write(address_width::word, 0x6f0c, 0x34010001); // li $at, 0x1
    io_write(address_width::word, 0x6f10, 0x0ff019e1); // jal 0xbfc06784
    io_write(address_width::word, 0x6f14, 0xaf81a9c0); // sw $at -0x5460($gp)
  }
}

static void add_cpu_time(psx::address_width width) {
  switch (width) {
    case psx::address_width::byte: psx::timing::add_cpu_time( 8); break; // 8
    case psx::address_width::half: psx::timing::add_cpu_time(14); break; // 8+6
    case psx::address_width::word: psx::timing::add_cpu_time(26); break; // 8+6+6+6
  }
}

uint32_t bios::io_read(address_width width, uint32_t address) {
  add_cpu_time(width);

  return memory_base::io_read(width, address);
}

void bios::io_write(address_width width, uint32_t address, uint32_t data) {
  add_cpu_time(width);

  return memory_base::io_write(width, address, data);
}
