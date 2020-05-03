#ifndef SPU_VOICE_HPP_
#define SPU_VOICE_HPP_

#include "spu/adpcm.hpp"
#include "spu/adsr.hpp"
#include "spu/volume.hpp"
#include "util/fifo.hpp"
#include "util/int.hpp"
#include "addressable.hpp"
#include "sound-ram.hpp"
#include "memory.hpp"

namespace psx::spu {

  struct voice_t {
    adsr_t adsr;
    volume_t volume_left;
    volume_t volume_right;

    uint16_t pitch;
    uint32_t phase;

    psx::util::fifo_t<int16_t, 4> decoder_fifo;
    uint32_t current_address;
    uint32_t loop_address;
    uint32_t start_address;
    int16_t last_samples[2];

    uint32_t start_delay;

    adpcm_header_t header;

    void put_header(uint16_t val);
    void put_sample(uint16_t val);

    int32_t raw_sample();
    int32_t apply_envelope(int32_t raw);

    void counter_step();
  };
}  // namespace psx::spu

#endif  // SPU_VOICE_HPP_
