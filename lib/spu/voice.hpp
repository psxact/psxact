#ifndef SPU_VOICE_HPP_
#define SPU_VOICE_HPP_

#include "spu/adpcm.hpp"
#include "spu/adsr-envelope.hpp"
#include "spu/volume.hpp"
#include "util/fifo.hpp"
#include "util/int.hpp"
#include "addressable.hpp"

namespace psx::spu {

  struct voice {
    adsr_envelope adsr;
    volume volume_left;
    volume volume_right;

    uint16_t pitch;
    uint32_t phase;

    psx::util::fifo<int16_t, 4> decoder_fifo;
    uint32_t current_address;
    uint32_t loop_address;
    uint32_t start_address;
    int16_t last_samples[2];

    uint32_t start_delay;

    adpcm_header header;

    void put_header(uint16_t val);
    void put_sample(uint16_t val);

    int32_t raw_sample();
    int32_t apply_envelope(int32_t raw);

    void counter_step();
  };
}

#endif
