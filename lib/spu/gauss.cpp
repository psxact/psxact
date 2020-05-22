#include "spu/gauss.hpp"

using namespace psx::spu;

const int16_t gauss::table[256][4] = {
  #define ENTRY(a, b, c, d) { a, b, c, d },
  #include "spu/gauss.inc"
  #undef ENTRY
};

int32_t gauss::filter(int index, int16_t a, int16_t b, int16_t c, int16_t d) {
  auto coeffs = table[index];
  auto result = 0;

  result += int32_t(a) * int32_t(coeffs[0]);
  result += int32_t(b) * int32_t(coeffs[1]);
  result += int32_t(c) * int32_t(coeffs[2]);
  result += int32_t(d) * int32_t(coeffs[3]);

  return result >> 15;
}
