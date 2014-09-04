#include "stdafx.h"
#include "r3051.h"
#include "cp0.h"
#include "..\bus\bus.h"

void r3051_init(struct r3051* processor) {
  processor->pc = 0xbfc00000;
  processor->lo = 0x00000000;
  processor->hi = 0x00000000;

  memset(&processor->ic, 0, sizeof(struct r3051_pipestage));
}

void r3051_step(struct r3051* processor) {
  r3051_stage_wb(processor);

  r3051_stage_dc(processor);
  processor->wb = processor->dc;

  r3051_stage_ex(processor);
  processor->dc = processor->ex;

  r3051_stage_rf(processor);
  processor->ex = processor->rf;

  r3051_stage_ic(processor);
  processor->rf = processor->ic;
}

uint32_t address_mask[5] = { 0, 0, 1, 1, 3 };
uint32_t data_mask[5] = { 0xff, 0xff, 0xffff, 0xffff, 0xffffffff };
uint32_t data_sign[5] = { 0x80, 0x00, 0x8000, 0x0000, 0x00000000 };

extern struct r3051_cp0* cp0;

void r3051_dcache_fetch(enum r3051_datatype datatype, uint32_t address, uint32_t* data) {
  if (address & address_mask[datatype]) {
    // todo: address exception
    return;
  }

  uint32_t safe;

  if (bus_fetch(address, &safe)) {
    // todo: bus exception
    return;
  }

  uint32_t mask = data_mask[datatype];
  uint32_t sign = data_sign[datatype];

  switch (address & 3) {
  case 0: safe = (safe >>  0); break;
  case 1: safe = (safe >>  8); break;
  case 2: safe = (safe >> 16); break;
  case 3: safe = (safe >> 24); break;
  }

  *data = ((safe & mask) ^ sign) - sign;
}

void r3051_dcache_store(enum r3051_datatype datatype, uint32_t address, uint32_t* data) {
  uint32_t safe;

  if (cp0->registers[12] & (1 << 16)) {
    if (*data) {
      printf("IsC Write: 0x%08x => 0x%08x\n", *data, address);
    }

    return; // IsC
  }

  if (datatype == WORD) {
    safe = *data;
  }
  else {
    if (bus_fetch(address, &safe)) {
      // todo: bus exception
      // todo: only do merging for non-word writes
    }

    uint32_t mask = data_mask[datatype];

    safe = (safe & ~mask) | (*data & mask);
  }

  if (bus_store(address, &safe)) {
    // todo: bus exception
    return;
  }
}

void r3051_icache_fetch(enum r3051_datatype datatype, uint32_t address, uint32_t* data) {
  if (address & address_mask[datatype]) {
    // todo: address exception
    return;
  }

  uint32_t safe;

  if (bus_fetch(address, &safe)) {
    // todo: bus exception
    return;
  }

  *data = safe;
}
