#ifndef BIOS_HPP_
#define BIOS_HPP_

#include "memory.hpp"

namespace psx {

  using bios_base_t = memory_t<kib(512)>;

  class bios_t : public bios_base_t {
  public:
    bios_t();
  };
}

#endif  // BIOS_HPP_
