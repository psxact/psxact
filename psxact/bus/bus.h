#pragma once

#include "stdafx.h"

class Gpu;

class Bus {
  Gpu& gpu;

public:
  Bus(Gpu&);
  ~Bus(void);

  uint32_t Fetch(uint32_t);
  void Store(uint32_t, uint32_t);
};
