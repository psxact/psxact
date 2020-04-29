#ifndef TIMING_HPP_
#define TIMING_HPP_

namespace psx {

  constexpr double NTSC_COLOR_CARRIER = 3'579'545.454545455;
  constexpr double NTSC_SCANLINE_LENGTH = 227.5;
  constexpr double NTSC_SCANLINE_FREQ = NTSC_COLOR_CARRIER / NTSC_SCANLINE_LENGTH;
  constexpr double NTSC_LINES_PER_FIELD = 262.5;
  constexpr double NTSC_LINES_PER_FRAME = 525;

  constexpr int CPU_FREQ = 33'868'800;
  constexpr int GPU_FREQ = 53'222'400;
  constexpr int SPU_FREQ = 44'100;

  constexpr int SPU_DIVIDER = CPU_FREQ / SPU_FREQ;

  constexpr double GPU_LINE_LENGTH = GPU_FREQ / NTSC_SCANLINE_FREQ;

  // These are included to check that the math is correct.
  constexpr double FRAME_RATE = ((GPU_FREQ / GPU_LINE_LENGTH) / NTSC_LINES_PER_FRAME);
  constexpr double FIELD_RATE = ((GPU_FREQ / GPU_LINE_LENGTH) / NTSC_LINES_PER_FIELD);

  static_assert( int(FRAME_RATE * 100) == 29'97 );
  static_assert( int(FIELD_RATE * 100) == 59'94 );

}  // namespace psx

#endif  // TIMING_HPP_
