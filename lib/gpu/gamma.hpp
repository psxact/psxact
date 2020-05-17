#ifndef GPU_GAMMA_HPP_
#define GPU_GAMMA_HPP_

#include "gpu/types.hpp"

namespace psx::gpu {

  class gamma_t {
    static const int lut[256];

  public:
    static void apply(color_t &color);
  };
}

#endif
