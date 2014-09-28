#include "stdafx.h"
#include "gpu.h"

#define FRAME_BUFFER_X 1024
#define FRAME_BUFFER_Y 512
#define PIXEL_WIDTH sizeof(uint16_t)

uint8_t vram[FRAME_BUFFER_X * FRAME_BUFFER_Y * PIXEL_WIDTH];

#define LO(a) (((a) >>  0) & 0xffff)
#define HI(a) (((a) >> 16) & 0xffff)
#define MAKE_WORD(l, h) (((h) << 16) | (l))

static void gpu_write_4bpp(uint32_t x, uint32_t y, uint32_t index) {
	int nybble = (x & 1) * 4;
	int address = (y * 2048) + (x / 2);

	vram[address] &= ~(0xf << nybble);
	vram[address] |=  (0xf << nybble) & index;
}

static void gpu_write_8bpp(uint32_t x, uint32_t y, uint32_t index) {
	int address = (y * 2048) + x;

	vram[address] = index;
}

static void gpu_write_16bpp(uint32_t x, uint32_t y, uint32_t color) {
	int address = (y * 2048) + (x * 2);

	vram[address + 0] = (color >> 0);
	vram[address + 1] = (color >> 8);
}

static void gpu_write_24bpp(uint32_t x, uint32_t y, uint32_t color) {
	int address = (y * 2048) + (x * 3);

	vram[address + 0] = (color >> 0);
	vram[address + 1] = (color >> 8);
	vram[address + 2] = (color >> 16);
}

static void gpu_cmd_40(uint32_t color, uint32_t point1, uint32_t point2) {
	struct coordinate s, e, t, d;

	s.x = LO(point1);
	s.y = HI(point1);
	e.x = LO(point2);
	e.y = HI(point2);

	t.x = (s.x << 16) + (1 << 15);
	t.y = (s.y << 16) + (1 << 15);
	d.x = (e.x - s.x);
	d.y = (e.y - s.y);

	uint32_t adx = abs(d.x);
	uint32_t ady = abs(d.y);
	uint32_t div = max(adx, ady);

	d.x = (d.x << 16) / div;
	d.y = (d.y << 16) / div;

	while (s.x != e.x || s.y != e.y) {
		gpu_write_24bpp(s.x, s.y, color);

		if (s.x != e.x) s.x = (t.x += d.x) >> 16;
		if (s.y != e.y) s.y = (t.y += d.y) >> 16;
	}
}

static void gpu_cmd_48(uint32_t color, uint32_t *points) {
	uint32_t index = 0;
	uint32_t point1 = points[index++];
	uint32_t point2 = points[index++];

	while (point2 != 0x55555555) {
		gpu_cmd_40(color, point1, point2);

		point1 = point2;
		point2 = points[index++];
	}
}

static void gpu_cmd_60(uint32_t color, uint32_t point, uint32_t bound) {
	uint32_t x, y, w, h;

	x = LO(point);
	y = HI(point);
	w = LO(bound) + x;
	h = HI(bound) + y;

	w = min(w, FRAME_BUFFER_X);
	h = min(h, FRAME_BUFFER_Y);

	for (; y <= h; y++) {
		for (; x <= w; x++) {
			gpu_write_24bpp(x, y, color);
		}
	}
}

static void gpu_cmd_68(uint32_t color, uint32_t point) {
	gpu_cmd_60(color, point, MAKE_WORD(1, 1));
}

static void gpu_cmd_70(uint32_t color, uint32_t point) {
	gpu_cmd_60(color, point, MAKE_WORD(8, 8));
}

static void gpu_cmd_78(uint32_t color, uint32_t point) {
	gpu_cmd_60(color, point, MAKE_WORD(16, 16));
}

void gpu_write_gp0(uint32_t data) {
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

void gpu_write_gp1(uint32_t data) {
}

uint32_t gpu_read_resp(void) {
	return 0;
}

uint32_t gpu_read_stat(void) {
	return 0x14802000;
}
