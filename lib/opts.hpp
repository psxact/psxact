#ifndef PSX_OPTS_HPP_
#define PSX_OPTS_HPP_

#include <optional>

namespace psx {

  enum class bios_patch {
    debug_tty,
    skip_intro
  };

  enum class component {
    bios,
    console,
    cpu,
    dcache,
    dma,
    exp1,
    exp2,
    exp3,
    gpu,
    spu,
    mdec,
    memctl,
    cdrom,
    input,
    timer,
    vram,
    wram
  };

  enum class game_type {
    /// Used for the case where a game isn't present.
    none,
    /// Used for the case where a game is a disc image.
    disc,
    /// Used for the case where a game is an executable.
    psexe
  };

  class opts {
    FILE *bios_file;
    std::optional<FILE *> game_file = std::nullopt;
    game_type game_file_type = game_type::none;
    uint32_t log_flags = 0;
    uint32_t bios_patch_flags = 0;

  public:
    static opts empty;

    opts(
      FILE *bios_file,
      std::optional<FILE *> game_file,
      game_type game_file_type,
      uint32_t log_flags,
      uint32_t bios_patch_flags
    );

    FILE *get_bios_file();
    std::optional<FILE *> get_game_file();
    game_type get_game_file_type();
    const char *get_log_name(component val);
    bool get_log_enabled(component val);
    bool get_patch_enabled(bios_patch val);
  };
}

#endif
