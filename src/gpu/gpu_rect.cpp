#include "gpu_core.hpp"
#include "../state.hpp"
#include "../memory/vram.hpp"

// Rect Commands
//
// 24    | Texture Mode      (0=Blended, 1=Raw)       (Textured only)
// 25    | Semi Transparency (0=Off, 1=On)

static uint32_t get_x_length(gpu_state_t *state) {
  switch ((state->fifo.buffer[0] >> 27) & 3) {
  case 0:
    return
      ((state->fifo.buffer[0] >> 26) & 1)
        ? uint16_t(state->fifo.buffer[3])
        : uint16_t(state->fifo.buffer[2]);

  case 1: return 1;
  case 2: return 8;
  case 3: return 16;
  }
}

static uint32_t get_y_length(gpu_state_t *state) {
  switch ((state->fifo.buffer[0] >> 27) & 3) {
  case 0:
    return
      ((state->fifo.buffer[0] >> 26) & 1)
        ? uint16_t(state->fifo.buffer[3] >> 16)
        : uint16_t(state->fifo.buffer[2] >> 16);

  case 1: return 1;
  case 2: return 8;
  case 3: return 16;
  }
}

static gpu::color_t color_from_halfword(uint16_t color) {
  gpu::color_t result;
  result.r = (color & 0x001f) << 3;
  result.g = (color & 0x03e0) >> 2;
  result.b = (color & 0x7c00) >> 7;

  return result;
}

static gpu::color_t get_texture_color_4bpp(gpu_state_t *state, int clut_x, int clut_y, int u, int v, int x, int y) {
  auto base_u = ((state->status >> 0) & 0xf) << 6;
  auto base_v = ((state->status >> 4) & 0x1) << 8;
  auto texel = vram::read(base_u + (x / 4),
                          base_v + y);

  auto index = (texel >> ((x & 3) * 4)) & 15;

  auto color = vram::read(clut_x + index,
                          clut_y);

  return color_from_halfword(color);
}

static gpu::color_t get_texture_color_8bpp(gpu_state_t *state, int clut_x, int clut_y, int u, int v, int x, int y) {
  auto base_u = ((state->status >> 0) & 0xf) << 6;
  auto base_v = ((state->status >> 4) & 0x1) << 8;

  u += x;
  v += y;

  auto texel = vram::read(base_u + (u / 2),
                          base_v + v);

  auto index = (texel >> ((x & 1) * 8)) & 0xff;

  auto color = vram::read(clut_x + index,
                          clut_y);

  return color_from_halfword(color);
}

static gpu::color_t get_texture_color(gpu_state_t *state, int x, int y) {
  uint32_t clut_x = (state->fifo.buffer[2] >> 12) & 0x3f0;
  uint32_t clut_y = (state->fifo.buffer[2] >> 22) & 0x1ff;

  uint32_t u = (state->fifo.buffer[2] >> 0) & 0xff;
  uint32_t v = (state->fifo.buffer[2] >> 8) & 0xff;

  switch ((state->status >> 7) & 3) {
  case 0:
    return get_texture_color_4bpp(state, clut_x, clut_y, u, v, x, y);

  case 1: // 8BPP
    return get_texture_color_8bpp(state, clut_x, clut_y, u, v, x, y);

  case 2:
  case 3: // 15BPP
    return {};
  }
}

static gpu::color_t get_color(gpu_state_t *state, int x, int y) {
  if (state->fifo.buffer[0] & (1 << 26)) {
    return get_texture_color(state, x, y);
  }
  else {
    gpu::color_t result;

    result.r = (state->fifo.buffer[0] >> (0 * 8)) & 0xff;
    result.g = (state->fifo.buffer[0] >> (1 * 8)) & 0xff;
    result.b = (state->fifo.buffer[0] >> (2 * 8)) & 0xff;

    return result;
  }
}

void gpu::draw_rect(gpu_state_t *state) {
  uint32_t xofs = state->x_offset + int16_t(state->fifo.buffer[1]);
  uint32_t yofs = state->y_offset + int16_t(state->fifo.buffer[1] >> 16);

  uint32_t w = get_x_length(state);
  uint32_t h = get_y_length(state);

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      auto color = get_color(state, x, y);

      gpu::draw_point(state,
                      xofs + x,
                      yofs + y,
                      color.r,
                      color.g,
                      color.b);
    }
  }
}
