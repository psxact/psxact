#include "gpu/gamma.hpp"

#include <cmath>
#include "args.hpp"

using namespace psx;
using namespace psx::gpu;

static constexpr int gamma(int x) {
  return std::sqrt(256) * std::pow(x, 1.1 / 2.2);
}

#define DO_0(x) gamma(x)
#define DO_1(x) DO_0(x + 0), DO_0(x + 1)
#define DO_2(x) DO_1(x + 0), DO_1(x + 2)
#define DO_3(x) DO_2(x + 0), DO_2(x + 4)
#define DO_4(x) DO_3(x + 0), DO_3(x + 8)
#define DO_5(x) DO_4(x + 0), DO_4(x + 16)
#define DO_6(x) DO_5(x + 0), DO_5(x + 32)
#define DO_7(x) DO_6(x + 0), DO_6(x + 64)

const int gamma_t::lut[256] = {
  DO_7(0)
};

void gamma_t::apply(color_t &color) {
  if (args::gamma_correction) {
    color.r = lut[color.r];
    color.g = lut[color.g];
    color.b = lut[color.b];
  }
}
