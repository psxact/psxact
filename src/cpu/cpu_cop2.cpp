#include <algorithm>
#include "cpu_cop2.hpp"

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

uint32_t cop2::divide(cop2_state_t *state) {
  auto &ccr = state->ccr;
  auto &gpr = state->gpr;

  if (gpr.sz[3] <= (ccr.h / 2)) {
    return flags::e(state);
  }

  uint32_t z = utility::clz<16>(gpr.sz[3]);
  uint64_t n = uint64_t(ccr.h) << z;
  uint32_t d = gpr.sz[3] << z;
  uint32_t u = unr_table[((d & 0x7fff) + 0x40) >> 7] + 0x101;

  d = (0x2000080 - (d * u)) >> (8 * 1);
  d = (0x0000080 + (d * u)) >> (8 * 1);
  d = (0x0008000 + (d * n)) >> (8 * 2);

  return std::min(d, 0x1ffffu);
}

void cop2::run(cop2_state_t *state, uint32_t code) {
  auto &ccr = state->ccr;

  ccr.flag = 0;

  switch (code & 0x3f) {
  case 0x00: cop2::op_rtps(state, code); break;
  case 0x01: cop2::op_rtps(state, code); break;
  case 0x06: cop2::op_nclip(state, code); break;
  case 0x0c: cop2::op_op(state, code); break;
  case 0x10: cop2::op_dpcs(state, code); break;
  case 0x11: cop2::op_intpl(state, code); break;
  case 0x12: cop2::op_mvmva(state, code); break;
  case 0x13: cop2::op_ncds(state, code); break;
  case 0x14: cop2::op_cdp(state, code); break;
  case 0x16: cop2::op_ncdt(state, code); break;
  case 0x1a: cop2::op_dcpl(state, code); break;
  case 0x1b: cop2::op_nccs(state, code); break;
  case 0x1c: cop2::op_cc(state, code); break;
  case 0x1e: cop2::op_ncs(state, code); break;
  case 0x20: cop2::op_nct(state, code); break;
  case 0x28: cop2::op_sqr(state, code); break;
  case 0x29: cop2::op_dcpl(state, code); break;
  case 0x2a: cop2::op_dpct(state, code); break;
  case 0x2d: cop2::op_avsz3(state, code); break;
  case 0x2e: cop2::op_avsz4(state, code); break;
  case 0x30: cop2::op_rtpt(state, code); break;
  case 0x3d: cop2::op_gpf(state, code); break;
  case 0x3e: cop2::op_gpl(state, code); break;
  case 0x3f: cop2::op_ncct(state, code); break;

  default:
    printf("cop2::run(0x%08x)\n", code);
    break;
  }

  bool msb = (ccr.flag & 0x7f87e000) != 0;
  ccr.flag = (ccr.flag | (msb << 31));
}

// -=======-
//  Helpers
// -=======-

enum {
  MX_ROT = 0,
  MX_LLM = 1,
  MX_LCM = 2,
  MX_NIL = 3
};

enum {
  CV_TR = 0,
  CV_BK = 1,
  CV_FC = 2,
  CV_ZR = 3
};

static inline int get_sf(uint32_t code) {
  return (code & (1 << 19)) ? 12 : 0;
}

static inline int get_mx(uint32_t code) {
  return (code >> 17) & 3;
}

static inline int get_v(uint32_t code) {
  return (code >> 15) & 3;
}

static inline int get_cv(uint32_t code) {
  return (code >> 13) & 3;
}

static inline int get_lm(uint32_t code) {
  return (code & (1 << 10)) ? 0 : (-32768);
}

static inline void mac_to_ir(cop2_state_t *state, uint32_t code) {
  auto &gpr = state->gpr;

  gpr.vector[3][0] = cop2::flags::b(state, 0, code, gpr.mac[1]);
  gpr.vector[3][1] = cop2::flags::b(state, 1, code, gpr.mac[2]);
  gpr.vector[3][2] = cop2::flags::b(state, 2, code, gpr.mac[3]);
}

static inline void mac_to_rgb(cop2_state_t *state) {
  auto &gpr = state->gpr;

  gpr.rgb[0] = gpr.rgb[1];
  gpr.rgb[1] = gpr.rgb[2];

  gpr.rgb[2].r = cop2::flags::c(state, 0, gpr.mac[1] >> 4);
  gpr.rgb[2].g = cop2::flags::c(state, 1, gpr.mac[2] >> 4);
  gpr.rgb[2].b = cop2::flags::c(state, 2, gpr.mac[3] >> 4);
  gpr.rgb[2].c = gpr.rgbc.c;
}

static inline void depth_cue(cop2_state_t *state, uint32_t code, int32_t r, int32_t g, int32_t b) {
  auto &gpr = state->gpr;

  int64_t rfc = int64_t(state->ccr.vector[CV_FC][0]) << 12;
  int64_t gfc = int64_t(state->ccr.vector[CV_FC][1]) << 12;
  int64_t bfc = int64_t(state->ccr.vector[CV_FC][2]) << 12;

  int sf = get_sf(code);

  gpr.mac[1] = cop2::flags::a(state, 0, rfc - r) >> sf;
  gpr.mac[1] = cop2::flags::a(state, 0, r + gpr.ir0 * cop2::flags::b(state, 0, 0, gpr.mac[1])) >> sf;

  gpr.mac[2] = cop2::flags::a(state, 1, gfc - g) >> sf;
  gpr.mac[2] = cop2::flags::a(state, 1, g + gpr.ir0 * cop2::flags::b(state, 1, 0, gpr.mac[2])) >> sf;

  gpr.mac[3] = cop2::flags::a(state, 2, bfc - b) >> sf;
  gpr.mac[3] = cop2::flags::a(state, 2, b + gpr.ir0 * cop2::flags::b(state, 2, 0, gpr.mac[3])) >> sf;

  mac_to_ir(state, code);
  mac_to_rgb(state);
}

static inline void transform_dq(cop2_state_t *state, int64_t div) {
  auto &ccr = state->ccr;
  auto &gpr = state->gpr;

  gpr.mac[0] = cop2::flags::f(state, ccr.dqb + ccr.dqa * div);
  gpr.ir0 = cop2::flags::h(state, (ccr.dqb + ccr.dqa * div) >> 12);
}

static inline void transform_xy(cop2_state_t *state, int64_t div) {
  auto &ccr = state->ccr;
  auto &gpr = state->gpr;

  gpr.mac[0] = cop2::flags::f(state, int64_t(ccr.ofx) + gpr.vector[3][0] * div) >> 16;

  gpr.sx[0] = gpr.sx[1];
  gpr.sx[1] = gpr.sx[2];
  gpr.sx[2] = cop2::flags::g(state, 0, gpr.mac[0]);

  gpr.mac[0] = cop2::flags::f(state, int64_t(ccr.ofy) + gpr.vector[3][1] * div) >> 16;

  gpr.sy[0] = gpr.sy[1];
  gpr.sy[1] = gpr.sy[2];
  gpr.sy[2] = cop2::flags::g(state, 1, gpr.mac[0]);
}

template<bool buggy = false>
static inline int64_t transform(cop2_state_t *state, uint32_t code, int mx, int cv, int v) {
  auto &ccr = state->ccr;
  auto &gpr = state->gpr;

  int64_t mac;

  auto &matrix = ccr.matrix[mx];
  auto &offset = ccr.vector[cv];
  auto &vector = gpr.vector[v];

  int shift = get_sf(code);

  for (int i = 0; i < 3; i++) {
    if (buggy) {

      int32_t mulr[3];

      int64_t mac = int64_t(offset[i]) << 12;

      if (mx == 3) {
        if (i == 0) {
          mulr[0] = -((gpr.rgbc.r << 4) * vector[0]);
          mulr[1] = (gpr.rgbc.r << 4) * vector[1];
          mulr[2] = gpr.ir0 * vector[2];
        }
        else {
          int cr = i == 1
                   ? ccr.matrix[MX_ROT][0][2]
                   : ccr.matrix[MX_ROT][1][1];

          mulr[0] = cr * vector[0];
          mulr[1] = cr * vector[1];
          mulr[2] = cr * vector[2];
        }
      }
      else {
        mulr[0] = matrix[i][0] * vector[0];
        mulr[1] = matrix[i][1] * vector[1];
        mulr[2] = matrix[i][2] * vector[2];
      }

      mac = cop2::flags::a(state, i, mac + mulr[0]);

      if (cv == CV_FC) {
        cop2::flags::b(state, i, 0, mac >> shift);
        mac = 0;
      }

      mac = cop2::flags::a(state, i, mac + mulr[1]);
      mac = cop2::flags::a(state, i, mac + mulr[2]);

      gpr.mac[1 + i] = int32_t(mac >> shift);
    }
    else {
      mac = int64_t(offset[i]) << 12;
      mac = cop2::flags::a(state, i, mac + (matrix[i][0] * vector[0]));
      mac = cop2::flags::a(state, i, mac + (matrix[i][1] * vector[1]));
      mac = cop2::flags::a(state, i, mac + (matrix[i][2] * vector[2]));

      gpr.mac[1 + i] = int32_t(mac >> shift);
    }
  }

  return mac;
}

static inline int64_t transform_pt(cop2_state_t *state, uint32_t code, int mx, int cv, int v) {
  auto &ccr = state->ccr;
  auto &gpr = state->gpr;

  int32_t z = int32_t(transform(state, code, mx, cv, v) >> 12);

  gpr.vector[3][0] = cop2::flags::b(state, 0, code, gpr.mac[1]);
  gpr.vector[3][1] = cop2::flags::b(state, 1, code, gpr.mac[2]);
  gpr.vector[3][2] = cop2::flags::b(state, 2, code, gpr.mac[3], z);

  gpr.sz[0] = gpr.sz[1];
  gpr.sz[1] = gpr.sz[2];
  gpr.sz[2] = gpr.sz[3];
  gpr.sz[3] = cop2::flags::d(state, z);

  return cop2::divide(state);
}

// -============-
//  Instructions
// -============-

void cop2::op_avsz3(cop2_state_t *state, uint32_t code) {
  auto &ccr = state->ccr;
  auto &gpr = state->gpr;

  int64_t temp = int64_t(ccr.zsf3) * (gpr.sz[1] + gpr.sz[2] + gpr.sz[3]);

  gpr.mac[0] = flags::f(state, temp);
  gpr.otz = flags::d(state, temp >> 12);
}

void cop2::op_avsz4(cop2_state_t *state, uint32_t code) {
  auto &ccr = state->ccr;
  auto &gpr = state->gpr;

  int64_t temp = int64_t(ccr.zsf4) * (gpr.sz[0] + gpr.sz[1] + gpr.sz[2] + gpr.sz[3]);

  gpr.mac[0] = flags::f(state, temp);
  gpr.otz = flags::d(state, temp >> 12);
}

void cop2::op_cc(cop2_state_t *state, uint32_t code) {
  transform(state, code, MX_LCM, CV_BK, 3);
  mac_to_ir(state, code);

  auto &gpr = state->gpr;

  int sf = get_sf(code);

  gpr.mac[1] = flags::a(state, 0, (gpr.rgbc.r << 4) * gpr.vector[3][0]) >> sf;
  gpr.mac[2] = flags::a(state, 1, (gpr.rgbc.g << 4) * gpr.vector[3][1]) >> sf;
  gpr.mac[3] = flags::a(state, 2, (gpr.rgbc.b << 4) * gpr.vector[3][2]) >> sf;

  mac_to_ir(state, code);
  mac_to_rgb(state);
}

void cop2::op_cdp(cop2_state_t *state, uint32_t code) {
  transform(state, code, MX_LCM, CV_BK, 3);
  mac_to_ir(state, code);

  auto &gpr = state->gpr;

  int32_t r = (gpr.rgbc.r << 4) * gpr.vector[3][0];
  int32_t g = (gpr.rgbc.g << 4) * gpr.vector[3][1];
  int32_t b = (gpr.rgbc.b << 4) * gpr.vector[3][2];

  depth_cue(state, code, r, g, b);
}

void cop2::op_dcpl(cop2_state_t *state, uint32_t code) {
  auto &gpr = state->gpr;

  int32_t r = (gpr.rgbc.r << 4) * gpr.vector[3][0];
  int32_t g = (gpr.rgbc.g << 4) * gpr.vector[3][1];
  int32_t b = (gpr.rgbc.b << 4) * gpr.vector[3][2];

  depth_cue(state, code, r, g, b);
}

void cop2::op_dpcs(cop2_state_t *state, uint32_t code) {
  auto &gpr = state->gpr;

  int32_t r = gpr.rgbc.r << 16;
  int32_t g = gpr.rgbc.g << 16;
  int32_t b = gpr.rgbc.b << 16;

  depth_cue(state, code, r, g, b);
}

void cop2::op_dpct(cop2_state_t *state, uint32_t code) {
  for (int i = 0; i < 3; i++) {
    auto &gpr = state->gpr;

    int32_t r = gpr.rgb[0].r << 16;
    int32_t g = gpr.rgb[0].g << 16;
    int32_t b = gpr.rgb[0].b << 16;

    depth_cue(state, code, r, g, b);
  }
}

void cop2::op_gpf(cop2_state_t *state, uint32_t code) {
  auto &gpr = state->gpr;

  int shift = get_sf(code);

  gpr.mac[1] = (gpr.ir0 * gpr.vector[3][0]) >> shift;
  gpr.mac[2] = (gpr.ir0 * gpr.vector[3][1]) >> shift;
  gpr.mac[3] = (gpr.ir0 * gpr.vector[3][2]) >> shift;

  mac_to_ir(state, code);
  mac_to_rgb(state);
}

void cop2::op_gpl(cop2_state_t *state, uint32_t code) {
  auto &gpr = state->gpr;

  int shift = get_sf(code);

  int64_t mac1 = int64_t(gpr.mac[1]) << shift;
  int64_t mac2 = int64_t(gpr.mac[2]) << shift;
  int64_t mac3 = int64_t(gpr.mac[3]) << shift;

  gpr.mac[1] = flags::a(state, 0, mac1 + (gpr.ir0 * gpr.vector[3][0])) >> shift;
  gpr.mac[2] = flags::a(state, 1, mac2 + (gpr.ir0 * gpr.vector[3][1])) >> shift;
  gpr.mac[3] = flags::a(state, 2, mac3 + (gpr.ir0 * gpr.vector[3][2])) >> shift;

  mac_to_ir(state, code);
  mac_to_rgb(state);
}

void cop2::op_intpl(cop2_state_t *state, uint32_t code) {
  auto &gpr = state->gpr;

  int64_t rfc = int64_t(state->ccr.vector[CV_FC][0]) << 12;
  int64_t gfc = int64_t(state->ccr.vector[CV_FC][1]) << 12;
  int64_t bfc = int64_t(state->ccr.vector[CV_FC][2]) << 12;

  int shift = get_sf(code);

  gpr.mac[1] = flags::a(state, 0, (rfc - (gpr.vector[3][0] << 12))) >> shift;
  gpr.mac[2] = flags::a(state, 1, (gfc - (gpr.vector[3][1] << 12))) >> shift;
  gpr.mac[3] = flags::a(state, 2, (bfc - (gpr.vector[3][2] << 12))) >> shift;

  gpr.mac[1] = flags::a(state, 0, ((int64_t(gpr.vector[3][0]) << 12) + gpr.ir0 * flags::b(state, 0, 0, gpr.mac[1])) >> shift);
  gpr.mac[2] = flags::a(state, 1, ((int64_t(gpr.vector[3][1]) << 12) + gpr.ir0 * flags::b(state, 1, 0, gpr.mac[2])) >> shift);
  gpr.mac[3] = flags::a(state, 2, ((int64_t(gpr.vector[3][2]) << 12) + gpr.ir0 * flags::b(state, 2, 0, gpr.mac[3])) >> shift);

  mac_to_ir(state, code);
  mac_to_rgb(state);
}

void cop2::op_mvmva(cop2_state_t *state, uint32_t code) {
  int mx = get_mx(code);
  int cv = get_cv(code);
  int v  = get_v (code);

  transform<1>(state, code, mx, cv, v);

  mac_to_ir(state, code);
}

void cop2::op_nccs(cop2_state_t *state, uint32_t code) {
  transform(state, code, MX_LLM, CV_ZR, 0);
  mac_to_ir(state, code);

  op_cc(state, code);
}

void cop2::op_ncct(cop2_state_t *state, uint32_t code) {
  for (int i = 0; i < 3; i++) {
    transform(state, code, MX_LLM, CV_ZR, i);
    mac_to_ir(state, code);

    op_cc(state, code);
  }
}

void cop2::op_ncds(cop2_state_t *state, uint32_t code) {
  transform(state, code, MX_LLM, CV_ZR, 0);
  mac_to_ir(state, code);

  transform(state, code, MX_LCM, CV_BK, 3);
  mac_to_ir(state, code);

  auto &gpr = state->gpr;

  int32_t r = (gpr.rgbc.r << 4) * gpr.vector[3][0];
  int32_t g = (gpr.rgbc.g << 4) * gpr.vector[3][1];
  int32_t b = (gpr.rgbc.b << 4) * gpr.vector[3][2];

  depth_cue(state, code, r, g, b);
}

void cop2::op_ncdt(cop2_state_t *state, uint32_t code) {
  for (int i = 0; i < 3; i++) {
    transform(state, code, MX_LLM, CV_ZR, i);
    mac_to_ir(state, code);

    transform(state, code, MX_LCM, CV_BK, 3);
    mac_to_ir(state, code);

    auto &gpr = state->gpr;

    int32_t r = (gpr.rgbc.r << 4) * gpr.vector[3][0];
    int32_t g = (gpr.rgbc.g << 4) * gpr.vector[3][1];
    int32_t b = (gpr.rgbc.b << 4) * gpr.vector[3][2];

    depth_cue(state, code, r, g, b);
  }
}

void cop2::op_nclip(cop2_state_t *state, uint32_t code) {
  auto &gpr = state->gpr;

  int64_t temp =
    (gpr.sx[0] * int64_t(gpr.sy[1] - gpr.sy[2])) +
    (gpr.sx[1] * int64_t(gpr.sy[2] - gpr.sy[0])) +
    (gpr.sx[2] * int64_t(gpr.sy[0] - gpr.sy[1]));

  gpr.mac[0] = flags::f(state, temp);
}

void cop2::op_ncs(cop2_state_t *state, uint32_t code) {
  transform(state, code, MX_LLM, CV_ZR, 0);
  mac_to_ir(state, code);

  transform(state, code, MX_LCM, CV_BK, 3);
  mac_to_ir(state, code);
  mac_to_rgb(state);
}

void cop2::op_nct(cop2_state_t *state, uint32_t code) {
  for (int i = 0; i < 3; i++) {
    transform(state, code, MX_LLM, CV_ZR, i);
    mac_to_ir(state, code);

    transform(state, code, MX_LCM, CV_BK, 3);
    mac_to_ir(state, code);
    mac_to_rgb(state);
  }
}

void cop2::op_op(cop2_state_t *state, uint32_t code) {
  int shift = get_sf(code);

  auto &matrix = state->ccr.matrix[MX_ROT];
  auto &gpr = state->gpr;

  gpr.mac[1] = ((matrix[1][1] * gpr.vector[3][2]) - (matrix[2][2] * gpr.vector[3][1])) >> shift;
  gpr.mac[2] = ((matrix[2][2] * gpr.vector[3][0]) - (matrix[0][0] * gpr.vector[3][2])) >> shift;
  gpr.mac[3] = ((matrix[0][0] * gpr.vector[3][1]) - (matrix[1][1] * gpr.vector[3][0])) >> shift;

  mac_to_ir(state, code);
}

void cop2::op_rtps(cop2_state_t *state, uint32_t code) {
  int64_t div = transform_pt(state, code, MX_ROT, CV_TR, 0);

  transform_xy(state, div);
  transform_dq(state, div);
}

void cop2::op_rtpt(cop2_state_t *state, uint32_t code) {
  int64_t div = 0;

  div = transform_pt(state, code, MX_ROT, CV_TR, 0);
  transform_xy(state, div);

  div = transform_pt(state, code, MX_ROT, CV_TR, 1);
  transform_xy(state, div);

  div = transform_pt(state, code, MX_ROT, CV_TR, 2);
  transform_xy(state, div);
  transform_dq(state, div);
}

void cop2::op_sqr(cop2_state_t *state, uint32_t code) {
  auto &gpr = state->gpr;

  int shift = get_sf(code);

  gpr.mac[1] = (gpr.vector[3][0] * gpr.vector[3][0]) >> shift;
  gpr.mac[2] = (gpr.vector[3][1] * gpr.vector[3][1]) >> shift;
  gpr.mac[3] = (gpr.vector[3][2] * gpr.vector[3][2]) >> shift;

  mac_to_ir(state, code);
}
