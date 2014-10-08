#pragma once

#include "stdafx.h"
#include "bus/bus.h"
#include "gpu/gpu.h"
#include "r3051/r3051.h"

class PlayStation {
  Bus* bus;
  Gpu* gpu;
  R3051* cpu;

public:
  PlayStation(void);
  ~PlayStation(void);

  bool LoadBiosImage(const char*);
  bool LoadDiskImage(const char*);
  void Step(void);
};
