#include "stdafx.h"
#include "r3051.h"
#include "..\bus\bus.h"

R3051::ICache icache;

uint32_t R3051::icacheFetch(uint32_t address) {
  uint32_t index = (address >> 4) & 0xff;
  uint32_t tag = (address >> 0) & 0xfffff000;
  uint32_t* line = icache.lines[index];

  if (icache.tag[index] != tag || icache.valid[index] == 0) {
    line[0] = bus_fetch((address & ~15) | 0);
    line[1] = bus_fetch((address & ~15) | 4);
    line[2] = bus_fetch((address & ~15) | 8);
    line[3] = bus_fetch((address & ~15) | 12);

    icache.tag[index] = tag;
    icache.valid[index] = 1;
  }

  return line[(address >> 2) & 3];
}

void R3051::icacheStore(uint32_t address, uint32_t data) {
  assert(0 && "ICache Store");
}
