#include "memory/bios.hpp"

#include "args.hpp"
#include "timing.hpp"

using namespace psx;
using namespace psx::memory;

bios::bios() : bios_base("bios") {
  load_blob(args::get_bios_file());

  if (args::get_patch_enabled(bios_patch::skip_intro)) {
    io_write(address_width::word, 0x6990, 0x00000000); // nop
  }

  if (args::get_patch_enabled(bios_patch::debug_tty)) {
    io_write(address_width::word, 0x6f0c, 0x34010001); // li $at, 0x1
    io_write(address_width::word, 0x6f10, 0x0ff019e1); // jal 0xbfc06784
    io_write(address_width::word, 0x6f14, 0xaf81a9c0); // sw $at -0x5460($gp)
  }
}

static void add_cpu_time(psx::address_width width) {
  constexpr int non_seq = 8;
  constexpr int seq = 6;

  switch (width) {
    case address_width::byte: return timing::add_cpu_time(non_seq);
    case address_width::half: return timing::add_cpu_time(non_seq + (seq * 1));
    case address_width::word: return timing::add_cpu_time(non_seq + (seq * 3));
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
