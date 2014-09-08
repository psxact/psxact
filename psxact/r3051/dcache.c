#include "stdafx.h"
#include "r3051.h"

struct r3051_dcache dcache;

uint32_t r3051_dcache_fetch(uint32_t address) {
  return dcache.lines[(address >> 2) & 0xff][0];
}

void r3051_dcache_store(uint32_t address, uint32_t data) {
  dcache.lines[(address >> 2) & 0xff][0] = data;
}
