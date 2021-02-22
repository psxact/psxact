#include "opts.hpp"

using namespace psx;

opts opts::empty(nullptr, std::nullopt, game_type::none, 0, 0);

opts::opts(
  FILE *bios_file,
  std::optional<FILE *> game_file,
  game_type game_file_type,
  uint32_t log_flags,
  uint32_t bios_patch_flags)
    : bios_file(bios_file)
    , game_file(game_file)
    , game_file_type(game_file_type)
    , log_flags(log_flags)
    , bios_patch_flags(bios_patch_flags) {
}

FILE *opts::get_bios_file() {
  return bios_file;
}

std::optional<FILE *> opts::get_game_file() {
  return game_file;
}

game_type opts::get_game_file_type() {
  return game_file_type;
}

const char *opts::get_log_name(component val) {
  switch (val) {
    case component::bios: return "bios";
    case component::console: return "console";
    case component::cpu: return "cpu";
    case component::dcache: return "dcache";
    case component::dma: return "dma";
    case component::exp1: return "exp1";
    case component::exp2: return "exp2";
    case component::exp3: return "exp3";
    case component::gpu: return "gpu";
    case component::spu: return "spu";
    case component::mdec: return "mdec";
    case component::memctl: return "memctl";
    case component::cdrom: return "cdrom";
    case component::input: return "input";
    case component::timer: return "timer";
    case component::vram: return "vram";
    case component::wram: return "wram";
  }

  return nullptr;
}

bool opts::get_log_enabled(component val) {
  return log_flags & (1 << int(val));
}

bool opts::get_patch_enabled(bios_patch val) {
  return bios_patch_flags & (1 << int(val));
}
