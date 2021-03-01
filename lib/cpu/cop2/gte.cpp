#include "cpu/cop2/gte.hpp"

#include <algorithm>
#include "util/uint.hpp"

using namespace psx::cpu::cop2;
using namespace psx::util;

static const uint8_t unr_table[0x101] = {
  0xff, 0xfd, 0xfb, 0xf9, 0xf7, 0xf5, 0xf3, 0xf1, 0xef, 0xee, 0xec, 0xea, 0xe8, 0xe6, 0xe4, 0xe3,
  0xe1, 0xdf, 0xdd, 0xdc, 0xda, 0xd8, 0xd6, 0xd5, 0xd3, 0xd1, 0xd0, 0xce, 0xcd, 0xcb, 0xc9, 0xc8,
  0xc6, 0xc5, 0xc3, 0xc1, 0xc0, 0xbe, 0xbd, 0xbb, 0xba, 0xb8, 0xb7, 0xb5, 0xb4, 0xb2, 0xb1, 0xb0,
  0xae, 0xad, 0xab, 0xaa, 0xa9, 0xa7, 0xa6, 0xa4, 0xa3, 0xa2, 0xa0, 0x9f, 0x9e, 0x9c, 0x9b, 0x9a,
  0x99, 0x97, 0x96, 0x95, 0x94, 0x92, 0x91, 0x90, 0x8f, 0x8d, 0x8c, 0x8b, 0x8a, 0x89, 0x87, 0x86,
  0x85, 0x84, 0x83, 0x82, 0x81, 0x7f, 0x7e, 0x7d, 0x7c, 0x7b, 0x7a, 0x79, 0x78, 0x77, 0x75, 0x74,
  0x73, 0x72, 0x71, 0x70, 0x6f, 0x6e, 0x6d, 0x6c, 0x6b, 0x6a, 0x69, 0x68, 0x67, 0x66, 0x65, 0x64,
  0x63, 0x62, 0x61, 0x60, 0x5f, 0x5e, 0x5d, 0x5d, 0x5c, 0x5b, 0x5a, 0x59, 0x58, 0x57, 0x56, 0x55,
  0x54, 0x53, 0x53, 0x52, 0x51, 0x50, 0x4f, 0x4e, 0x4d, 0x4d, 0x4c, 0x4b, 0x4a, 0x49, 0x48, 0x48,
  0x47, 0x46, 0x45, 0x44, 0x43, 0x43, 0x42, 0x41, 0x40, 0x3f, 0x3f, 0x3e, 0x3d, 0x3c, 0x3c, 0x3b,
  0x3a, 0x39, 0x39, 0x38, 0x37, 0x36, 0x36, 0x35, 0x34, 0x33, 0x33, 0x32, 0x31, 0x31, 0x30, 0x2f,
  0x2e, 0x2e, 0x2d, 0x2c, 0x2c, 0x2b, 0x2a, 0x2a, 0x29, 0x28, 0x28, 0x27, 0x26, 0x26, 0x25, 0x24,
  0x24, 0x23, 0x22, 0x22, 0x21, 0x20, 0x20, 0x1f, 0x1e, 0x1e, 0x1d, 0x1d, 0x1c, 0x1b, 0x1b, 0x1a,
  0x19, 0x19, 0x18, 0x18, 0x17, 0x16, 0x16, 0x15, 0x15, 0x14, 0x14, 0x13, 0x12, 0x12, 0x11, 0x11,
  0x10, 0x0f, 0x0f, 0x0e, 0x0e, 0x0d, 0x0d, 0x0c, 0x0c, 0x0b, 0x0a, 0x0a, 0x09, 0x09, 0x08, 0x08,
  0x07, 0x07, 0x06, 0x06, 0x05, 0x05, 0x04, 0x04, 0x03, 0x03, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00,
  0x00
};

uint32_t gte::divide() {
  if (gpr.sz[3] <= (ccr.h / 2)) {
    return flag_e();
  }

  uint32_t z = uint_t<16>::clz(gpr.sz[3]);
  uint64_t n = uint64_t(ccr.h) << z;
  uint32_t d = gpr.sz[3] << z;
  uint32_t u = unr_table[((d & 0x7fff) + 0x40) >> 7] + 0x101;

  d = uint32_t((0x2000080 - (d * u)) >> (8 * 1));
  d = uint32_t((0x0000080 + (d * u)) >> (8 * 1));
  d = uint32_t((0x0008000 + (d * n)) >> (8 * 2));

  return std::min(d, 0x1ffffu);
}

gte::gte() : logger("gte", false) {
}

void gte::run(uint32_t code) {
  ccr.flag = 0;

  switch (code & 0x3f) {
    case 0x00: op_rtps(code); break;
    case 0x01: op_rtps(code); break;
    case 0x06: op_nclip(); break;
    case 0x0c: op_op(code); break;
    case 0x10: op_dpcs(code); break;
    case 0x11: op_intpl(code); break;
    case 0x12: op_mvmva(code); break;
    case 0x13: op_ncds(code); break;
    case 0x14: op_cdp(code); break;
    case 0x16: op_ncdt(code); break;
    case 0x1a: op_dcpl(code); break;
    case 0x1b: op_nccs(code); break;
    case 0x1c: op_cc(code); break;
    case 0x1e: op_ncs(code); break;
    case 0x20: op_nct(code); break;
    case 0x28: op_sqr(code); break;
    case 0x29: op_dcpl(code); break;
    case 0x2a: op_dpct(code); break;
    case 0x2d: op_avsz3(); break;
    case 0x2e: op_avsz4(); break;
    case 0x30: op_rtpt(code); break;
    case 0x3d: op_gpf(code); break;
    case 0x3e: op_gpl(code); break;
    case 0x3f: op_ncct(code); break;

    default:
      LOG_INFO("cop2::run(0x%08x)\n", code);
      break;
  }

  bool msb = (ccr.flag & 0x7f87e000) != 0;
  ccr.flag = (ccr.flag | (msb << 31));
}

// -=======-
//  Helpers
// -=======-

typedef int32_t matrix_data[3][3];
typedef int32_t vector_data[3];

static inline int32_t get_sf(uint32_t code) {
  return (code & (1 << 19)) ? 12 : 0;
}

matrix gte::get_mx(uint32_t code) {
  return matrix((code >> 17) & 3);
}

static inline int32_t get_v(uint32_t code) {
  return (code >> 15) & 3;
}

vector gte::get_cv(uint32_t code) {
  return vector((code >> 13) & 3);
}

void gte::mac_to_ir(uint32_t code) {
  gpr.vector[3][0] = flag_b(0, code, gpr.mac[1]);
  gpr.vector[3][1] = flag_b(1, code, gpr.mac[2]);
  gpr.vector[3][2] = flag_b(2, code, gpr.mac[3]);
}

void gte::mac_to_rgb() {
  gpr.rgb[0] = gpr.rgb[1];
  gpr.rgb[1] = gpr.rgb[2];

  gpr.rgb[2].r = flag_c(0, gpr.mac[1] >> 4);
  gpr.rgb[2].g = flag_c(1, gpr.mac[2] >> 4);
  gpr.rgb[2].b = flag_c(2, gpr.mac[3] >> 4);
  gpr.rgb[2].c = gpr.rgbc.c;
}

void gte::depth_cue(uint32_t code, int32_t r, int32_t g, int32_t b) {
  int64_t rfc = int64_t(ccr.vector[static_cast<int>(vector::fc)][0]) << 12;
  int64_t gfc = int64_t(ccr.vector[static_cast<int>(vector::fc)][1]) << 12;
  int64_t bfc = int64_t(ccr.vector[static_cast<int>(vector::fc)][2]) << 12;

  int32_t shift = get_sf(code);

  gpr.mac[1] = int32_t(flag_a(0, rfc - r) >> shift);
  gpr.mac[1] = int32_t(flag_a(0, r + gpr.ir0 * flag_b(0, 0, gpr.mac[1])) >> shift);

  gpr.mac[2] = int32_t(flag_a(1, gfc - g) >> shift);
  gpr.mac[2] = int32_t(flag_a(1, g + gpr.ir0 * flag_b(1, 0, gpr.mac[2])) >> shift);

  gpr.mac[3] = int32_t(flag_a(2, bfc - b) >> shift);
  gpr.mac[3] = int32_t(flag_a(2, b + gpr.ir0 * flag_b(2, 0, gpr.mac[3])) >> shift);

  mac_to_ir(code);
  mac_to_rgb();
}

void gte::transform_dq(int64_t div) {
  gpr.mac[0] = int32_t(flag_f(ccr.dqb + ccr.dqa * div));
  gpr.ir0 = flag_h((ccr.dqb + ccr.dqa * div) >> 12);
}

void gte::transform_xy(int64_t div) {
  gpr.mac[0] = int32_t(flag_f(int64_t(ccr.ofx) + gpr.vector[3][0] * div) >> 16);

  gpr.sx[0] = gpr.sx[1];
  gpr.sx[1] = gpr.sx[2];
  gpr.sx[2] = flag_g(0, gpr.mac[0]);

  gpr.mac[0] = int32_t(flag_f(int64_t(ccr.ofy) + gpr.vector[3][1] * div) >> 16);

  gpr.sy[0] = gpr.sy[1];
  gpr.sy[1] = gpr.sy[2];
  gpr.sy[2] = flag_g(1, gpr.mac[0]);
}

int64_t gte::transform(uint32_t code, matrix mx, vector cv, int32_t v) {
  int64_t mac = 0;

  matrix_data &matrix = ccr.matrix[static_cast<int>(mx)];
  vector_data &offset = ccr.vector[static_cast<int>(cv)];
  vector_data &vector = gpr.vector[v];

  int32_t shift = get_sf(code);

  for (int32_t i = 0; i < 3; i++) {
    mac = int64_t(offset[i]) << 12;
    mac = flag_a(i, mac + (matrix[i][0] * vector[0]));
    mac = flag_a(i, mac + (matrix[i][1] * vector[1]));
    mac = flag_a(i, mac + (matrix[i][2] * vector[2]));

    gpr.mac[1 + i] = int32_t(mac >> shift);
  }

  return mac;
}

int64_t gte::transform_buggy(uint32_t code, matrix mx, vector cv, int32_t v) {
  int64_t mac = 0;

  matrix_data &matrix = ccr.matrix[static_cast<int>(mx)];
  vector_data &offset = ccr.vector[static_cast<int>(cv)];
  vector_data &vector = gpr.vector[v];

  int32_t shift = get_sf(code);

  for (int32_t i = 0; i < 3; i++) {
    int32_t mulr[3];

    int64_t mac = int64_t(offset[i]) << 12;

    if (mx == matrix::nil) {
      if (i == 0) {
        mulr[0] = -((gpr.rgbc.r << 4) * vector[0]);
        mulr[1] = (gpr.rgbc.r << 4) * vector[1];
        mulr[2] = gpr.ir0 * vector[2];
      } else {
        int32_t cr =
          i == 1
            ? ccr.matrix[static_cast<int>(matrix::rot)][0][2]
            : ccr.matrix[static_cast<int>(matrix::rot)][1][1];

        mulr[0] = cr * vector[0];
        mulr[1] = cr * vector[1];
        mulr[2] = cr * vector[2];
      }
    } else {
      mulr[0] = matrix[i][0] * vector[0];
      mulr[1] = matrix[i][1] * vector[1];
      mulr[2] = matrix[i][2] * vector[2];
    }

    mac = flag_a(i, mac + mulr[0]);

    if (cv == vector::fc) {
      flag_b(i, 0, int32_t(mac >> shift));
      mac = 0;
    }

    mac = flag_a(i, mac + mulr[1]);
    mac = flag_a(i, mac + mulr[2]);

    gpr.mac[1 + i] = int32_t(mac >> shift);
  }

  return mac;
}

int64_t gte::transform_pt(uint32_t code, matrix mx, vector cv, int32_t v) {
  int32_t z = int32_t(transform(code, mx, cv, v) >> 12);

  gpr.vector[3][0] = flag_b(0, code, gpr.mac[1]);
  gpr.vector[3][1] = flag_b(1, code, gpr.mac[2]);
  gpr.vector[3][2] = flag_b(2, code, gpr.mac[3], z);

  gpr.sz[0] = gpr.sz[1];
  gpr.sz[1] = gpr.sz[2];
  gpr.sz[2] = gpr.sz[3];
  gpr.sz[3] = flag_d(z);

  return divide();
}

// -============-
//  Instructions
// -============-

void gte::op_avsz3() {
  int64_t temp = int64_t(ccr.zsf3) * (gpr.sz[1] + gpr.sz[2] + gpr.sz[3]);

  gpr.mac[0] = int32_t(flag_f(temp));
  gpr.otz = flag_d(int32_t(temp >> 12));
}

void gte::op_avsz4() {
  int64_t temp = int64_t(ccr.zsf4) * (gpr.sz[0] + gpr.sz[1] + gpr.sz[2] + gpr.sz[3]);

  gpr.mac[0] = int32_t(flag_f(temp));
  gpr.otz = flag_d(int32_t(temp >> 12));
}

void gte::op_cc(uint32_t code) {
  transform(code, matrix::lcm, vector::bk, 3);
  mac_to_ir(code);

  int32_t shift = get_sf(code);

  gpr.mac[1] = int32_t(flag_a(0, (gpr.rgbc.r << 4) * gpr.vector[3][0]) >> shift);
  gpr.mac[2] = int32_t(flag_a(1, (gpr.rgbc.g << 4) * gpr.vector[3][1]) >> shift);
  gpr.mac[3] = int32_t(flag_a(2, (gpr.rgbc.b << 4) * gpr.vector[3][2]) >> shift);

  mac_to_ir(code);
  mac_to_rgb();
}

void gte::op_cdp(uint32_t code) {
  transform(code, matrix::lcm, vector::bk, 3);
  mac_to_ir(code);

  int32_t r = (gpr.rgbc.r << 4) * gpr.vector[3][0];
  int32_t g = (gpr.rgbc.g << 4) * gpr.vector[3][1];
  int32_t b = (gpr.rgbc.b << 4) * gpr.vector[3][2];

  depth_cue(code, r, g, b);
}

void gte::op_dcpl(uint32_t code) {
  int32_t r = (gpr.rgbc.r << 4) * gpr.vector[3][0];
  int32_t g = (gpr.rgbc.g << 4) * gpr.vector[3][1];
  int32_t b = (gpr.rgbc.b << 4) * gpr.vector[3][2];

  depth_cue(code, r, g, b);
}

void gte::op_dpcs(uint32_t code) {
  int32_t r = gpr.rgbc.r << 16;
  int32_t g = gpr.rgbc.g << 16;
  int32_t b = gpr.rgbc.b << 16;

  depth_cue(code, r, g, b);
}

void gte::op_dpct(uint32_t code) {
  for (int32_t i = 0; i < 3; i++) {
    int32_t r = gpr.rgb[0].r << 16;
    int32_t g = gpr.rgb[0].g << 16;
    int32_t b = gpr.rgb[0].b << 16;

    depth_cue(code, r, g, b);
  }
}

void gte::op_gpf(uint32_t code) {
  int32_t shift = get_sf(code);

  gpr.mac[1] = (gpr.ir0 * gpr.vector[3][0]) >> shift;
  gpr.mac[2] = (gpr.ir0 * gpr.vector[3][1]) >> shift;
  gpr.mac[3] = (gpr.ir0 * gpr.vector[3][2]) >> shift;

  mac_to_ir(code);
  mac_to_rgb();
}

void gte::op_gpl(uint32_t code) {
  int32_t shift = get_sf(code);

  int64_t mac1 = int64_t(gpr.mac[1]) << shift;
  int64_t mac2 = int64_t(gpr.mac[2]) << shift;
  int64_t mac3 = int64_t(gpr.mac[3]) << shift;

  gpr.mac[1] = int32_t(flag_a(0, mac1 + (gpr.ir0 * gpr.vector[3][0])) >> shift);
  gpr.mac[2] = int32_t(flag_a(1, mac2 + (gpr.ir0 * gpr.vector[3][1])) >> shift);
  gpr.mac[3] = int32_t(flag_a(2, mac3 + (gpr.ir0 * gpr.vector[3][2])) >> shift);

  mac_to_ir(code);
  mac_to_rgb();
}

void gte::op_intpl(uint32_t code) {
  auto fc = ccr.vector[static_cast<int>(vector::fc)];

  int64_t rfc = int64_t(fc[0]) << 12;
  int64_t gfc = int64_t(fc[1]) << 12;
  int64_t bfc = int64_t(fc[2]) << 12;

  int32_t shift = get_sf(code);

  gpr.mac[1] = int32_t(flag_a(0, rfc - (gpr.vector[3][0] << 12)) >> shift);
  gpr.mac[2] = int32_t(flag_a(1, gfc - (gpr.vector[3][1] << 12)) >> shift);
  gpr.mac[3] = int32_t(flag_a(2, bfc - (gpr.vector[3][2] << 12)) >> shift);

  gpr.mac[1] =
      int32_t(flag_a(0, ((int64_t(gpr.vector[3][0]) << 12) + gpr.ir0 * flag_b(0, 0, gpr.mac[1])) >> shift));

  gpr.mac[2] =
      int32_t(flag_a(1, ((int64_t(gpr.vector[3][1]) << 12) + gpr.ir0 * flag_b(1, 0, gpr.mac[2])) >> shift));

  gpr.mac[3] =
      int32_t(flag_a(2, ((int64_t(gpr.vector[3][2]) << 12) + gpr.ir0 * flag_b(2, 0, gpr.mac[3])) >> shift));

  mac_to_ir(code);
  mac_to_rgb();
}

void gte::op_mvmva(uint32_t code) {
  matrix mx = get_mx(code);
  vector cv = get_cv(code);
  int32_t v = get_v(code);

  transform_buggy(code, mx, cv, v);

  mac_to_ir(code);
}

void gte::op_nccs(uint32_t code) {
  transform(code, matrix::llm, vector::zr, 0);
  mac_to_ir(code);

  op_cc(code);
}

void gte::op_ncct(uint32_t code) {
  for (int32_t i = 0; i < 3; i++) {
    transform(code, matrix::llm, vector::zr, i);
    mac_to_ir(code);

    op_cc(code);
  }
}

void gte::op_ncds(uint32_t code) {
  transform(code, matrix::llm, vector::zr, 0);
  mac_to_ir(code);

  transform(code, matrix::lcm, vector::bk, 3);
  mac_to_ir(code);

  int32_t r = (gpr.rgbc.r << 4) * gpr.vector[3][0];
  int32_t g = (gpr.rgbc.g << 4) * gpr.vector[3][1];
  int32_t b = (gpr.rgbc.b << 4) * gpr.vector[3][2];

  depth_cue(code, r, g, b);
}

void gte::op_ncdt(uint32_t code) {
  for (int32_t i = 0; i < 3; i++) {
    transform(code, matrix::llm, vector::zr, i);
    mac_to_ir(code);

    transform(code, matrix::lcm, vector::bk, 3);
    mac_to_ir(code);

    int32_t r = (gpr.rgbc.r << 4) * gpr.vector[3][0];
    int32_t g = (gpr.rgbc.g << 4) * gpr.vector[3][1];
    int32_t b = (gpr.rgbc.b << 4) * gpr.vector[3][2];

    depth_cue(code, r, g, b);
  }
}

void gte::op_nclip() {
  int64_t temp =
    (gpr.sx[0] * int64_t(gpr.sy[1] - gpr.sy[2])) +
    (gpr.sx[1] * int64_t(gpr.sy[2] - gpr.sy[0])) +
    (gpr.sx[2] * int64_t(gpr.sy[0] - gpr.sy[1]));

  gpr.mac[0] = int32_t(flag_f(temp));
}

void gte::op_ncs(uint32_t code) {
  transform(code, matrix::llm, vector::zr, 0);
  mac_to_ir(code);

  transform(code, matrix::lcm, vector::bk, 3);
  mac_to_ir(code);
  mac_to_rgb();
}

void gte::op_nct(uint32_t code) {
  for (int32_t i = 0; i < 3; i++) {
    transform(code, matrix::llm, vector::zr, i);
    mac_to_ir(code);

    transform(code, matrix::lcm, vector::bk, 3);
    mac_to_ir(code);
    mac_to_rgb();
  }
}

void gte::op_op(uint32_t code) {
  matrix_data &matrix = ccr.matrix[static_cast<int>(matrix::rot)];

  int32_t shift = get_sf(code);

  gpr.mac[1] = ((matrix[1][1] * gpr.vector[3][2]) - (matrix[2][2] * gpr.vector[3][1])) >> shift;
  gpr.mac[2] = ((matrix[2][2] * gpr.vector[3][0]) - (matrix[0][0] * gpr.vector[3][2])) >> shift;
  gpr.mac[3] = ((matrix[0][0] * gpr.vector[3][1]) - (matrix[1][1] * gpr.vector[3][0])) >> shift;

  mac_to_ir(code);
}

void gte::op_rtps(uint32_t code) {
  int64_t div = transform_pt(code, matrix::rot, vector::tr, 0);

  transform_xy(div);
  transform_dq(div);
}

void gte::op_rtpt(uint32_t code) {
  int64_t div = 0;

  for (int i = 0; i < 3; i++) {
    div = transform_pt(code, matrix::rot, vector::tr, i);
    transform_xy(div);
  }

  transform_dq(div);
}

void gte::op_sqr(uint32_t code) {
  int32_t shift = get_sf(code);

  gpr.mac[1] = (gpr.vector[3][0] * gpr.vector[3][0]) >> shift;
  gpr.mac[2] = (gpr.vector[3][1] * gpr.vector[3][1]) >> shift;
  gpr.mac[3] = (gpr.vector[3][2] * gpr.vector[3][2]) >> shift;

  mac_to_ir(code);
}
