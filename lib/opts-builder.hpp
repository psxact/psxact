#ifndef PSX_OPTS_BUILDER_HPP_
#define PSX_OPTS_BUILDER_HPP_

#include "opts.hpp"

namespace psx {

  class opts_builder {
    std::optional<FILE *> bios_file = std::nullopt;
    std::optional<FILE *> game_file = std::nullopt;
    game_type game_file_type = game_type::none;
    uint32_t log_flags = 0;
    uint32_t bios_patch_flags = 0;

  public:
    void put_bios_filename(const char *val);
    void put_game_filename(const char *val);
    void put_log_enabled(component val);
    void put_patch_enabled(bios_patch val);

    std::optional<opts> build() const;
  };
}

#endif
