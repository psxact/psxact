#pragma once

#include "stdafx.h"

class Gpu {
  void Write4bpp (uint32_t, uint32_t, uint32_t);
  void Write8bpp (uint32_t, uint32_t, uint32_t);
  void Write16bpp(uint32_t, uint32_t, uint32_t);
  void Write24bpp(uint32_t, uint32_t, uint32_t);

  void Cmd40(uint32_t, uint32_t, uint32_t);
  void Cmd48(uint32_t, uint32_t*);
  void Cmd60(uint32_t, uint32_t, uint32_t);
  void Cmd68(uint32_t, uint32_t);
  void Cmd70(uint32_t, uint32_t);
  void Cmd78(uint32_t, uint32_t);

public:
  void WriteGp0(uint32_t);
  void WriteGp1(uint32_t);

  uint32_t ReadResp(void);
  uint32_t ReadStat(void);

  struct coordinate {
    int16_t x;
    int16_t y;

    coordinate(void) {
    }

    coordinate(int16_t x, int16_t y) {
      this->x = x;
      this->y = y;
    }
  };
};
