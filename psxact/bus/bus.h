#pragma once

#include "stdafx.h"

class Bus {
public:
  uint32_t Fetch(uint32_t);
  void Store(uint32_t, uint32_t);
};
