#ifndef CDROM_XA_ADPCM_HPP_
#define CDROM_XA_ADPCM_HPP_

#include <cstdint>
#include <tuple>

#include "cdrom/cdrom-sector.hpp"
#include "cdrom/resampler.hpp"

namespace psx::cdrom {
  
  class xa_adpcm_t {
    resampler_t left {};
    resampler_t right {};
    int16_t prev_samples_left[2] {};
    int16_t prev_samples_right[2] {};

  public:
    std::tuple<int16_t, int16_t> read();

    void decode(const cdrom_sector_t &sector);
    void decode_segment(const cdrom_sector_t &sector, int segment);
    void decode_sample_pair(uint8_t header_left, uint8_t left, uint8_t header_right, uint8_t right);
    int16_t decode_sample(uint8_t header, uint8_t sample, int16_t (&prev)[2]);
  };
}

#endif // CDROM_XA_ADPCM_HPP_
