#include "stdafx.h"
#include "r3051.h"

R3051::DCache dcache;

uint32_t R3051::DCacheFetch(uint32_t address) {
  return dcache.lines[(address >> 2) & 0xff][0];
}

void R3051::DCacheStore(uint32_t address, uint32_t data) {
  dcache.lines[(address >> 2) & 0xff][0] = data;
}
