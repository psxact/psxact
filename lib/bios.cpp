#include "bios.hpp"

#include "args.hpp"

using namespace psx;

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
