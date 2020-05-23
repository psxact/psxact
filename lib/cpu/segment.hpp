#ifndef CPU_SEGMENT_HPP_
#define CPU_SEGMENT_HPP_

#include <cstdint>

namespace psx::cpu {

  enum class segment {
    kuseg,
    kseg0,
    kseg1,
    kseg2
  };

  segment get_segment(uint32_t address);
  uint32_t get_segment_mask(uint32_t address);
  bool is_segment_cached(uint32_t address);
}

#endif // CPU_SEGMENT_HPP_
