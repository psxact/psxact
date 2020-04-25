#ifndef SPU_ADPCM_HPP_
#define SPU_ADPCM_HPP_

#include <cstdint>

namespace psx::spu {

  struct adpcm_header_t {
    int shift; // 0..3
    int filter; // 4..7
    bool loop_end; // 8
    bool loop_repeat; // 9
    bool loop_start; // 10

    static adpcm_header_t create(uint16_t header);
  };

  struct adpcm_sample_t {
    int16_t coded[4];

    static adpcm_sample_t create(uint16_t sample);
  };
}

#endif  // SPU_ADPCM_HPP_
