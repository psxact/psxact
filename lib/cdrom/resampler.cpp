#include "cdrom/resampler.hpp"

#include <cassert>
#include "util/int.hpp"

using namespace psx::cdrom;
using namespace psx::util;

static const int16_t table[7][29] = {
  { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,-0x0002, 0x000a,-0x0022,
    0x0041,-0x0054, 0x0034, 0x0009,-0x010a, 0x0400,-0x0a78, 0x234c,
    0x6794,-0x1780, 0x0bcd,-0x0623, 0x0350,-0x016d, 0x006b, 0x000a,
   -0x0010, 0x0011,-0x0008, 0x0003,-0x0001 },
  { 0x0000, 0x0000, 0x0000,-0x0002, 0x0000, 0x0003,-0x0013, 0x003c,
   -0x004b, 0x00a2,-0x00e3, 0x0132,-0x0043,-0x0267, 0x0c9d, 0x74bb,
   -0x11b4, 0x09b8,-0x05bf, 0x0372,-0x01a8, 0x00a6,-0x001b, 0x0005,
    0x0006,-0x0008, 0x0003,-0x0001, 0x0000 },
  { 0x0000, 0x0000,-0x0001, 0x0003,-0x0002,-0x0005, 0x001f,-0x004a,
    0x00b3,-0x0192, 0x02b1,-0x039e, 0x04f8,-0x05a6, 0x7939,-0x05a6,
    0x04f8,-0x039e, 0x02b1,-0x0192, 0x00b3,-0x004a, 0x001f,-0x0005,
   -0x0002, 0x0003,-0x0001, 0x0000, 0x0000 },
  { 0x0000,-0x0001, 0x0003,-0x0008, 0x0006, 0x0005,-0x001b, 0x00a6,
   -0x01a8, 0x0372,-0x05bf, 0x09b8,-0x11b4, 0x74bb, 0x0c9d,-0x0267,
   -0x0043, 0x0132,-0x00e3, 0x00a2,-0x004b, 0x003c,-0x0013, 0x0003,
    0x0000,-0x0002, 0x0000, 0x0000, 0x0000 },
  {-0x0001, 0x0003,-0x0008, 0x0011,-0x0010, 0x000a, 0x006b,-0x016d,
    0x0350,-0x0623, 0x0bcd,-0x1780, 0x6794, 0x234c,-0x0a78, 0x0400,
   -0x010a, 0x0009, 0x0034,-0x0054, 0x0041,-0x0022, 0x000a,-0x0001,
    0x0000, 0x0001, 0x0000, 0x0000, 0x0000 },
  { 0x0002,-0x0008, 0x0010,-0x0023, 0x002b, 0x001a,-0x00eb, 0x027b,
   -0x0548, 0x0afa,-0x16fa, 0x53e0, 0x3c07,-0x1249, 0x080e,-0x0347,
    0x015b,-0x0044,-0x0017, 0x0046,-0x0023, 0x0011,-0x0005, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
  {-0x0005, 0x0011,-0x0023, 0x0046,-0x0017,-0x0044, 0x015b,-0x0347,
    0x080e,-0x1249, 0x3c07, 0x53e0,-0x16fa, 0x0afa,-0x0548, 0x027b,
   -0x00eb, 0x001a, 0x002b,-0x0023, 0x0010,-0x0008, 0x0002, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }
};

void resampler::interpolate(int phase) {
  int sum = 0;

  for (int i = 0; i < 29; i++) {
    sum += (buffer[(offset - i) & 31] * table[phase][i]) / 0x8000;
  }

  put_output(int_t<16>::clamp(sum));
}

void resampler::put_sample(int16_t val) {
  buffer[offset & 31] = val;
  count6--;

  assert(count6 >= 0);

  if (count6 == 0) {
    count6 = 6;
    interpolate(0);
    interpolate(1);
    interpolate(2);
    interpolate(3);
    interpolate(4);
    interpolate(5);
    interpolate(6);
  }

  offset++;
}

int16_t resampler::get_output() {
  if (output_read_index == output_write_index) {
    return 0;
  }

  auto sample = output[output_read_index & 0xfff];
  output_read_index++;

  return sample;
}

void resampler::put_output(int16_t val) {
  output[output_write_index & 0xfff] = val;
  output_write_index++;
}
