#include <cstdint>
#include "cpu_cop2.hpp"

static uint32_t read_matrix_vector_group(cop2_state_t *state, uint32_t n) {
  auto &matrix = state->ccr.matrix[n >> 3];
  auto &vector = state->ccr.vector[n >> 3];

  switch (n & 7) {
  case 0: return uint16_t(matrix[0][0]) | (matrix[0][1] << 16);
  case 1: return uint16_t(matrix[0][2]) | (matrix[1][0] << 16);
  case 2: return uint16_t(matrix[1][1]) | (matrix[1][2] << 16);
  case 3: return uint16_t(matrix[2][0]) | (matrix[2][1] << 16);
  case 4: return uint32_t(matrix[2][2]);
  case 5: return uint32_t(vector[0]);
  case 6: return uint32_t(vector[1]);
  case 7: return uint32_t(vector[2]);

  default:
    return 0;
  }
}

uint32_t cop2::read_ccr(cop2_state_t *state, uint32_t n) {
  if (n <= 0x17) {
    return read_matrix_vector_group(state, n);
  }

  auto &ccr = state->ccr;

  switch (n) {
  case 0x18:
    return uint32_t(ccr.ofx);

  case 0x19:
    return uint32_t(ccr.ofy);

  case 0x1a:
    return int16_t(ccr.h);

  case 0x1b:
    return uint32_t(ccr.dqa);

  case 0x1c:
    return uint32_t(ccr.dqb);

  case 0x1d:
    return uint32_t(ccr.zsf3);

  case 0x1e:
    return uint32_t(ccr.zsf4);

  case 0x1f:
    return ccr.flag;

  default:
    return 0;
  }
}

static void write_matrix_vector_group(cop2_state_t *state, uint32_t n, uint32_t value) {
  auto &matrix = state->ccr.matrix[n >> 3];
  auto &vector = state->ccr.vector[n >> 3];

  switch (n & 7) {
  case 0:
    matrix[0][0] = int16_t(value);
    matrix[0][1] = int16_t(value >> 16);
    break;

  case 1:
    matrix[0][2] = int16_t(value);
    matrix[1][0] = int16_t(value >> 16);
    break;

  case 2:
    matrix[1][1] = int16_t(value);
    matrix[1][2] = int16_t(value >> 16);
    break;

  case 3:
    matrix[2][0] = int16_t(value);
    matrix[2][1] = int16_t(value >> 16);
    break;

  case 4:
    matrix[2][2] = int16_t(value);
    break;

  case 5:
    vector[0] = int32_t(value);
    break;

  case 6:
    vector[1] = int32_t(value);
    break;

  case 7:
    vector[2] = int32_t(value);
    break;
  }
}

void cop2::write_ccr(cop2_state_t *state, uint32_t n, uint32_t value) {
  if (n <= 0x17) {
    return write_matrix_vector_group(state, n, value);
  }

  auto &ccr = state->ccr;

  switch (n) {
  case 0x18:
    ccr.ofx = int32_t(value);
    break;

  case 0x19:
    ccr.ofy = int32_t(value);
    break;

  case 0x1a:
    ccr.h = uint16_t(value);
    break;

  case 0x1b:
    ccr.dqa = int16_t(value);
    break;

  case 0x1c:
    ccr.dqb = int32_t(value);
    break;

  case 0x1d:
    ccr.zsf3 = int16_t(value);
    break;

  case 0x1e:
    ccr.zsf4 = int16_t(value);
    break;

  case 0x1f: {
    bool msb = (value & 0x7f87e000) != 0;
    ccr.flag = (value & 0x7ffff000) | (msb << 31);
    break;
  }
  }
}

uint32_t cop2::read_gpr(cop2_state_t *state, uint32_t n) {
  auto &gpr = state->gpr;

  switch (n) {
  case 0x00:
    return uint16_t(gpr.vector[0][0]) | (uint16_t(gpr.vector[0][1]) << 16);

  case 0x01:
    return uint32_t(gpr.vector[0][2]);

  case 0x02:
    return uint16_t(gpr.vector[1][0]) | (uint16_t(gpr.vector[1][1]) << 16);

  case 0x03:
    return uint32_t(gpr.vector[1][2]);

  case 0x04:
    return uint16_t(gpr.vector[2][0]) | (uint16_t(gpr.vector[2][1]) << 16);

  case 0x05:
    return uint32_t(gpr.vector[2][2]);

  case 0x06:
    return uint32_t(gpr.rgbc.value);

  case 0x07:
    return uint32_t(gpr.otz);

  case 0x08:
    return uint32_t(gpr.ir0);

  case 0x09:
    return uint32_t(gpr.vector[3][0]);

  case 0x0a:
    return uint32_t(gpr.vector[3][1]);

  case 0x0b:
    return uint32_t(gpr.vector[3][2]);

  case 0x0c:
    return uint16_t(gpr.sx[0]) | (uint16_t(gpr.sy[0]) << 16);

  case 0x0d:
    return uint16_t(gpr.sx[1]) | (uint16_t(gpr.sy[1]) << 16);

  case 0x0e:
  case 0x0f:
    return uint16_t(gpr.sx[2]) | (uint16_t(gpr.sy[2]) << 16);

  case 0x10:
    return uint32_t(gpr.sz[0]);

  case 0x11:
    return uint32_t(gpr.sz[1]);

  case 0x12:
    return uint32_t(gpr.sz[2]);

  case 0x13:
    return uint32_t(gpr.sz[3]);

  case 0x14:
    return uint32_t(gpr.rgb[0].value);

  case 0x15:
    return uint32_t(gpr.rgb[1].value);

  case 0x16:
    return uint32_t(gpr.rgb[2].value);

  case 0x17:
    return uint32_t(gpr.res);

  case 0x18:
    return uint32_t(gpr.mac[0]);

  case 0x19:
    return uint32_t(gpr.mac[1]);

  case 0x1a:
    return uint32_t(gpr.mac[2]);

  case 0x1b:
    return uint32_t(gpr.mac[3]);

  case 0x1c:
  case 0x1d:
    return uint32_t(
        (utility::sclamp<0, 31>(gpr.vector[3][0] >> 7) <<  0) |
        (utility::sclamp<0, 31>(gpr.vector[3][1] >> 7) <<  5) |
        (utility::sclamp<0, 31>(gpr.vector[3][2] >> 7) << 10)
    );

  case 0x1e:
    return uint32_t(gpr.lzcs);

  case 0x1f:
    return uint32_t(gpr.lzcr);

  default:
    return 0;
  }
}

void cop2::write_gpr(cop2_state_t *state, uint32_t n, uint32_t value) {
  auto &gpr = state->gpr;

  switch (n) {
  case 0x00:
    gpr.vector[0][0] = int16_t(value);
    gpr.vector[0][1] = int16_t(value >> 16);
    break;

  case 0x01:
    gpr.vector[0][2] = int16_t(value);
    break;

  case 0x02:
    gpr.vector[1][0] = int16_t(value);
    gpr.vector[1][1] = int16_t(value >> 16);
    break;

  case 0x03:
    gpr.vector[1][2] = int16_t(value);
    break;

  case 0x04:
    gpr.vector[2][0] = int16_t(value);
    gpr.vector[2][1] = int16_t(value >> 16);
    break;

  case 0x05:
    gpr.vector[2][2] = int16_t(value);
    break;

  case 0x06:
    gpr.rgbc.value = uint32_t(value);
    break;

  case 0x07:
    gpr.otz = uint16_t(value);
    break;

  case 0x08:
    gpr.ir0 = int16_t(value);
    break;

  case 0x09:
    gpr.vector[3][0] = int16_t(value);
    break;

  case 0x0a:
    gpr.vector[3][1] = int16_t(value);
    break;

  case 0x0b:
    gpr.vector[3][2] = int16_t(value);
    break;

  case 0x0c:
    gpr.sx[0] = int16_t(value);
    gpr.sy[0] = int16_t(value >> 16);
    break;

  case 0x0d:
    gpr.sx[1] = int16_t(value);
    gpr.sy[1] = int16_t(value >> 16);
    break;

  case 0x0e:
    gpr.sx[2] = int16_t(value);
    gpr.sy[2] = int16_t(value >> 16);
    break;

  case 0x0f:
    gpr.sx[0] = gpr.sx[1];
    gpr.sx[1] = gpr.sx[2];
    gpr.sx[2] = int16_t(value);

    gpr.sy[0] = gpr.sy[1];
    gpr.sy[1] = gpr.sy[2];
    gpr.sy[2] = int16_t(value >> 16);
    break;

  case 0x10:
    gpr.sz[0] = uint16_t(value);
    break;

  case 0x11:
    gpr.sz[1] = uint16_t(value);
    break;

  case 0x12:
    gpr.sz[2] = uint16_t(value);
    break;

  case 0x13:
    gpr.sz[3] = uint16_t(value);
    break;

  case 0x14:
    gpr.rgb[0].value = uint32_t(value);
    break;

  case 0x15:
    gpr.rgb[1].value = uint32_t(value);
    break;

  case 0x16:
    gpr.rgb[2].value = uint32_t(value);
    break;

  case 0x17:
    gpr.res = int32_t(value);
    break;

  case 0x18:
    gpr.mac[0] = int32_t(value);
    break;

  case 0x19:
    gpr.mac[1] = int32_t(value);
    break;

  case 0x1a:
    gpr.mac[2] = int32_t(value);
    break;

  case 0x1b:
    gpr.mac[3] = int32_t(value);
    break;

  case 0x1c:
    gpr.vector[3][0] = (value & 0x001f) << 7;
    gpr.vector[3][1] = (value & 0x03e0) << 2;
    gpr.vector[3][2] = (value & 0x7c00) >> 3;
    break;

  case 0x1d:
    break;

  case 0x1e:
    gpr.lzcs = int32_t(value);
    gpr.lzcr = gpr.lzcs < 0
               ? utility::clz<32>(~value)
               : utility::clz<32>( value);
    break;

  case 0x1f:
    break;
  }
}
