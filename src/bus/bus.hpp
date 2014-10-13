#pragma once

#include "stdafx.hpp"
#include "../r3051/r3051.hpp"
#include "../gpu/gpu.hpp"
#include "../spu/spu.hpp"

class Bus {
  uint8_t* bios;
  uint8_t* disk;
  uint8_t  wram[0x200000];

  R3051* cpu;
  Gpu* gpu;
  Spu* spu;

public:
  Bus(R3051*, Gpu*, Spu*);
  ~Bus(void);

  uint32_t Fetch(uint32_t);
  void Store(uint32_t, uint32_t);

  uint8_t** GetBios(void);
  uint8_t** GetDisk(void);
};
