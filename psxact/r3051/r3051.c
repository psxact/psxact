#include "stdafx.h"
#include "r3051.h"

void r3051_init(struct r3051* processor) {
  processor->pc = 0xbfc00000;
  processor->lo = 0x00000000;
  processor->hi = 0x00000000;

  memset(&processor->ic, 0, sizeof(struct r3051_pipestage));
}

void r3051_step(struct r3051* processor) {
  r3051_stage_dc(processor);

  r3051_stage_ex(processor);
  processor->dc = processor->ex;

  r3051_stage_rf(processor);
  processor->ex = processor->rf;

  r3051_stage_ic(processor);
  processor->rf = processor->ic;
}

uint8_t* bios;
uint8_t* disk;
uint8_t  wram[0x200000];

bool r3051_dcache_fetch(uint32_t address, uint32_t* data) {
  if (address >= 0x1f800000 && address <= 0x1f8003ff) {
    printf("Scratchpad Read: [%08x]\n", address);
    return 0;
  }

  if (address >= 0x1f801000 && address <= 0x1f801fff) {
    printf("I/O Read: [%08x]\n", address);
    return 0;
  }

  if (address >= 0x80000000 && address <= 0x801fffff) {
    *data = *((uint32_t*)(wram + (address & 0x1ffffc)));
    return 0;
  }

  if (address >= 0xa0000000 && address <= 0xa01fffff) {
    *data = *((uint32_t*)(wram + (address & 0x1ffffc)));
    return 0;
  }

  printf("Unknown D-Cache Read: [%08x]\n", address);
  return 1;
}

bool r3051_dcache_store(uint32_t address, uint32_t* data) {
  if (address == 0xbfc06f0c) {
    printf("woo\n");
  }

  if (address >= 0x00000000 && address <= 0x001fffff) {
    *((uint32_t*)(wram + (address & 0x1ffffc))) = *data;
    return 0;
  }

  if (address >= 0x1f800000 && address <= 0x1f8003ff) {
    printf("Scratchpad Write: [%08x] <= %08x\n", address, *data);
    return 0;
  }

  if (address >= 0x1f801000 && address <= 0x1f801fff) {
    printf("I/O Write: [%08x] <= %08x\n", address, *data);
    return 0;
  }

  if (address >= 0x1f802000 && address <= 0x1f802fff) {
    printf("EXP2 Write: [%08x] <= %08x\n", address, *data);
    return 0;
  }

  if (address >= 0x80000000 && address <= 0x801fffff) {
    *((uint32_t*)(wram + (address & 0x1ffffc))) = *data;
    return 0;
  }

  if (address >= 0xa0000000 && address <= 0xa01fffff) {
    *((uint32_t*)(wram + (address & 0x1ffffc))) = *data;
    return 0;
  }

  if (address == 0xfffe0130) {
    printf("Cache Control Register: [%08x] <= %08x\n", address, *data);
    return 0;
  }

  printf("Unknown D-Cache Write: [%08x] <= %08x\n", address, *data);
  return 1;
}

bool r3051_icache_fetch(uint32_t address, uint32_t* data) {
  if (address >= 0xbfc00000 && address <= 0xbfffffff) {
    address = (address & 0x7fffc);

    *data =
      (bios[address | 0] <<  0) |
      (bios[address | 1] <<  8) |
      (bios[address | 2] << 16) |
      (bios[address | 3] << 24);

    return true;
  }

  //
  // signal a cache error on un-mapped addresses

  printf("Unknown I-Cache Read: [%08x]\n", address);
  return false;
}
