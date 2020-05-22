#ifndef SPU_GAUSS_HPP_
#define SPU_GAUSS_HPP_

#include <cstdint>

namespace psx::spu {

  class gauss {
    static const int16_t table[256][4];

  public:
    static int32_t filter(int index, int16_t a, int16_t b, int16_t c, int16_t d);
  };
}

#endif
