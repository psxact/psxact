#ifndef CDROM_RESAMPLER_HPP_
#define CDROM_RESAMPLER_HPP_

#include <cstdint>

namespace psx::cdrom {
  class resampler_t {
    int16_t buffer[32] {};
    int32_t offset {0};
    int32_t count6 {6};

    int16_t output[4096] {};
    int32_t output_read_index {0};
    int32_t output_write_index {0};

    void interpolate(int phase);

  public:
    resampler_t() : count6(6) {}

    int16_t get_output();
    void put_output(int16_t val);
    void put_sample(int16_t val);
  };
} // namespace psx::cdrom


#endif // CDROM_RESAMPLER_HPP_
