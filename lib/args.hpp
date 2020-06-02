#ifndef ARGS_HPP_
#define ARGS_HPP_

#include <cstdio>
#include <optional>

namespace psx {
  enum class bios_patch {
    debug_tty,
    skip_intro
  };

  enum class component {
    cpu,
    dma,
    gpu,
    spu,
    mdec,
    cdrom,
    input,
    timer
  };

  enum class game_type {
    /// Used for the case where a game isn't present.
    none,
    /// Used for the case where a game is a disc image.
    disc,
    /// Used for the case where a game is an executable.
    psexe
  };

  class args {
    static void put_bios_filename(const char *val);
    static void put_game_filename(const char *val);
    static void put_log_enabled(component val);
    static void put_patch_enabled(bios_patch val);

  public:
    static void init(int argc, char **argv);

    static FILE *get_bios_file();
    static std::optional<FILE *> get_game_file();
    static game_type get_game_file_type();
    static bool get_log_enabled(component val);
    static bool get_patch_enabled(bios_patch val);
  };
}

#endif
