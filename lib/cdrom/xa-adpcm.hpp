#ifndef CDROM_XA_ADPCM_HPP_
#define CDROM_XA_ADPCM_HPP_

#include <cstdint>
#include <tuple>

#include "cdrom/cdrom-sector.hpp"
#include "cdrom/resampler.hpp"

namespace psx::cdrom {

  class xa_adpcm_t {
    resampler_t channel[2] {};
    int16_t prev[2][2] {};

  public:
    std::tuple<int16_t, int16_t> read();

    void decode(const cdrom_sector_t &sector);
    void decode_segment(const cdrom_sector_t &sector, int segment);
    void decode_sample(uint8_t header, int16_t sample, int32_t channel);
  };
}

#endif // CDROM_XA_ADPCM_HPP_
