#include "stdafx.hpp"
#include "gpu.hpp"
#include "../bus/bus.hpp"

#define FRAME_BUFFER_X 1024
#define FRAME_BUFFER_Y 512
#define PIXEL_WIDTH sizeof(uint16_t)

uint8_t vram[FRAME_BUFFER_X * FRAME_BUFFER_Y * PIXEL_WIDTH];

#define LO(a) (((a) >>  0) & 0xffff)
#define HI(a) (((a) >> 16) & 0xffff)
#define MAKE_WORD(l, h) (((h) << 16) | (l))

void Gpu::Write4bpp(uint32_t x, uint32_t y, uint32_t index) {
  int nybble = (x & 1) * 4;
  int address = (y * 2048) + (x / 2);

  vram[address] &= ~(0xf << nybble);
  vram[address] |=  (0xf << nybble) & index;
}

void Gpu::Write8bpp(uint32_t x, uint32_t y, uint32_t index) {
  int address = (y * 2048) + x;

  vram[address] = index;
}

void Gpu::Write16bpp(uint32_t x, uint32_t y, uint32_t color) {
  int address = (y * 2048) + (x * 2);

  vram[address + 0] = (color >> 0);
  vram[address + 1] = (color >> 8);
}

void Gpu::Write24bpp(uint32_t x, uint32_t y, uint32_t color) {
  int address = (y * 2048) + (x * 3);

  vram[address + 0] = (color >> 0);
  vram[address + 1] = (color >> 8);
  vram[address + 2] = (color >> 16);
}

void Gpu::Cmd40(uint32_t color, uint32_t point1, uint32_t point2) {
  Coordinate s(LO(point1), HI(point1));
  Coordinate e(LO(point2), HI(point2));

  if (point1 == point2) {
    Write24bpp(s.x, s.y, color);
    return;
  }

  Coordinate t, d;
  t.x = (s.x << 16) + (1 << 15);
  t.y = (s.y << 16) + (1 << 15);
  d.x = (e.x - s.x);
  d.y = (e.y - s.y);

  int16_t div = std::max(
    std::abs(d.x),
    std::abs(d.y));

  d.x = (d.x << 16) / div;
  d.y = (d.y << 16) / div;

  while (s.x != e.x || s.y != e.y) {
    Write24bpp(s.x, s.y, color);

    if (s.x != e.x) s.x = (t.x += d.x) >> 16;
    if (s.y != e.y) s.y = (t.y += d.y) >> 16;
  }
}

void Gpu::Cmd48(uint32_t color, uint32_t *points) {
  uint32_t index = 0;
  uint32_t point1 = points[index++];
  uint32_t point2 = points[index++];

  while (point2 != 0x55555555) {
    Cmd40(color, point1, point2);

    point1 = point2;
    point2 = points[index++];
  }
}

void Gpu::Cmd60(uint32_t color, uint32_t point, uint32_t bound) {
  int32_t x, y, w, h;

  x = LO(point);
  y = HI(point);
  w = LO(bound) + x;
  h = HI(bound) + y;

  w = std::min(w, FRAME_BUFFER_X);
  h = std::min(h, FRAME_BUFFER_Y);

  for (; y <= h; y++) {
    for (; x <= w; x++) {
      Write24bpp(x, y, color);
    }
  }
}

void Gpu::Cmd68(uint32_t color, uint32_t point) {
  Cmd60(color, point, MAKE_WORD(1, 1));
}

void Gpu::Cmd70(uint32_t color, uint32_t point) {
  Cmd60(color, point, MAKE_WORD(8, 8));
}

void Gpu::Cmd78(uint32_t color, uint32_t point) {
  Cmd60(color, point, MAKE_WORD(16, 16));
}

void Gpu::AttachBus(Bus* bus) {
  this->bus = bus;
}

void Gpu::WriteGp0(uint32_t data) {
  switch (data >> 24) {
  case 0x20: // Monochrome three - point polygon, opaque
  case 0x22: // Monochrome three - point polygon, semi - transparent
  case 0x28: // Monochrome four - point polygon, opaque
  case 0x2a: // Monochrome four - point polygon, semi - transparent

  case 0x24: // Textured three - point polygon, opaque, texture - blending
  case 0x25: // Textured three - point polygon, opaque, raw - texture
  case 0x26: // Textured three - point polygon, semi - transparent, texture - blending
  case 0x27: // Textured three - point polygon, semi - transparent, raw - texture
  case 0x2c: // Textured four - point polygon, opaque, texture - blending
  case 0x2d: // Textured four - point polygon, opaque, raw - texture
  case 0x2e: // Textured four - point polygon, semi - transparent, texture - blending
  case 0x2f: // Textured four - point polygon, semi - transparent, raw - texture

  case 0x30: // Shaded three - point polygon, opaque
  case 0x32: // Shaded three - point polygon, semi - transparent
  case 0x38: // Shaded four - point polygon, opaque
  case 0x3a: // Shaded four - point polygon, semi - transparent

  case 0x34: // Shaded Textured three - point polygon, opaque, texture - blending
  case 0x36: // Shaded Textured three - point polygon, semi - transparent, tex - blend
  case 0x3c: // Shaded Textured four - point polygon, opaque, texture - blending
  case 0x3e: // Shaded Textured four - point polygon, semi - transparent, tex - blend

  case 0x40: // Monochrome line, opaque
  case 0x42: // Monochrome line, semi-transparent
  case 0x48: // Monochrome Poly-line, opaque
  case 0x4a: // Monochrome Poly-line, semi-transparent

  case 0x50: // Shaded line, opaque
  case 0x52: // Shaded line, semi-transparent
  case 0x58: // Shaded Poly-line, opaque
  case 0x5a: // Shaded Poly-line, semi-transparent
    break;
  }
}

void Gpu::WriteGp1(uint32_t data) {
}

uint32_t Gpu::ReadResp(void) {
  return 0;
}

uint32_t Gpu::ReadStat(void) {
  return 0x14802000;
}

Gpu::Coordinate::Coordinate(void)
  : Coordinate(0, 0) {
}

Gpu::Coordinate::Coordinate(int16_t x, int16_t y)
  : x(x)
  , y(y) {
}
