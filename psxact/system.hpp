#pragma once

#include "stdafx.hpp"
#include "bus/bus.hpp"
#include "gpu/gpu.hpp"
#include "spu/spu.hpp"
#include "r3051/r3051.hpp"

class PlayStation {
  Bus* bus;
  Gpu* gpu;
  Spu* spu;
  R3051* cpu;

public:
  PlayStation(void);
  ~PlayStation(void);

  bool LoadBiosImage(const char*);
  bool LoadDiskImage(const char*);
  void Step(void);
};
