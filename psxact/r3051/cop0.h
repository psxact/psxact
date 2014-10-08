#pragma once

#include "stdafx.h"
#include "r3051.h"

class Cop0 {
  uint32_t registers[16];

public:
  Cop0(void);
  ~Cop0(void);

  uint32_t FetchSr(uint32_t);
  void StoreSr(uint32_t, uint32_t);

  uint32_t FetchCr(uint32_t);
  void StoreCr(uint32_t, uint32_t);

  bool Execute(uint32_t);
  void SysCall(R3051*);
};
