#include "opts-builder.hpp"

#include <cstring>

#include "util/panic.hpp"

using namespace psx;

void opts_builder::put_bios_filename(const char *val) {
  if (FILE *file = fopen(val, "rb")) {
    bios_file.emplace(file);
  } else {
    PANIC("Unable to open '%s' for reading.", val);
  }
}

void opts_builder::put_game_filename(const char *val) {
  if (FILE *file = fopen(val, "rb")) {
    game_file.emplace(file);

    if (strstr(val, ".exe") || strstr(val, ".psexe")) {
      game_file_type = game_type::psexe;
    } else {
      game_file_type = game_type::disc;
    }
  } else {
    PANIC("Unable to open '%s' for reading.", val);
  }
}

void opts_builder::put_log_enabled(component val) {
  log_flags |= 1 << int(val);
}

void opts_builder::put_patch_enabled(bios_patch val) {
  bios_patch_flags |= 1 << int(val);
}

std::optional<opts> opts_builder::build() const {
  if (!bios_file.has_value()) {
    return std::nullopt;
  }

  return opts(bios_file.value(), game_file, game_file_type, log_flags, bios_patch_flags);
}
