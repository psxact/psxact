#ifndef TIMING_HPP_
#define TIMING_HPP_

#include <cstdint>

namespace psx {
  constexpr double NTSC_COLOR_CARRIER = 3'579'545.454545455;
  constexpr double NTSC_SCANLINE_LENGTH = 227.5;
  constexpr double NTSC_SCANLINE_FREQ = NTSC_COLOR_CARRIER / NTSC_SCANLINE_LENGTH;
  constexpr double NTSC_LINES_PER_FIELD = 262.5;
  constexpr double NTSC_LINES_PER_FRAME = 525;
  constexpr double NTSC_FRAME_RATE = NTSC_SCANLINE_FREQ / NTSC_LINES_PER_FRAME;
  constexpr double NTSC_FIELD_RATE = NTSC_SCANLINE_FREQ / NTSC_LINES_PER_FIELD;

  // These are included to check that the math is correct.
  static_assert( int(NTSC_FRAME_RATE * 100) == 29'97 );
  static_assert( int(NTSC_FIELD_RATE * 100) == 59'94 );

  constexpr int CPU_FREQ = 33'868'800;
  constexpr int GPU_FREQ = 53'222'400;
  constexpr int SPU_FREQ = 44'100;

  constexpr int SPU_DIVIDER = CPU_FREQ / SPU_FREQ;

  constexpr double GPU_LINE_LENGTH = 3'379.2; // GPU_FREQ / NTSC_SCANLINE_FREQ;

  namespace timing {
    void add_cpu_time(int32_t amount);
    int32_t get_cpu_time();
    void reset_cpu_time();
  }
}

#endif
