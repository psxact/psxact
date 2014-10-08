#pragma once

#include "stdafx.h"

class Gpu;

class Bus {
  uint8_t* bios;
  uint8_t* disk;

  Gpu* gpu;

public:
  Bus(Gpu*);
  ~Bus(void);

  uint32_t Fetch(uint32_t);
  void Store(uint32_t, uint32_t);

  uint8_t** GetBios(void);
  uint8_t** GetDisk(void);
};
