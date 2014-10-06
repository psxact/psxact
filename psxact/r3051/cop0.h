#pragma once

#include "stdafx.h"
#include "r3051.h"

struct Cop0 {
  uint32_t registers[16];

  void init(void);
  void kill(void);

  uint32_t fetch_sr(uint32_t);
  void store_sr(uint32_t, uint32_t);

  uint32_t fetch_cr(uint32_t);
  void store_cr(uint32_t, uint32_t);

  void syscall(R3051*);
};
