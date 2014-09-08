#include "stdafx.h"
#include "r3051.h"
#include "cop0.h"
#include "..\bus\bus.h"

extern struct r3051_cop0* cop0;

uint32_t address_mask[5] = { 0x00000000, 0x00000000, 0x00000001, 0x00000001, 0x00000003 };

struct r3051_segment kuseg = {
  0x00000000, // start
  0x80000000, // length
  0x40000000, // offset
  true        // cached
};

struct r3051_segment kseg0 = {
  0x80000000, // start
  0x20000000, // length
  0x00000000, // offset
  true        // cached
};

struct r3051_segment kseg1 = {
  0xa0000000, // start
  0x20000000, // length
  0x00000000, // offset
  false       // cached
};

struct r3051_segment kseg2 = {
  0xc0000000, // start
  0x40000000, // length
  0x00000000, // offset
  false       // cached
};

struct r3051_segment* segments[8] = {
  &kuseg, // user segment
  0,      // invalid
  0,      // invalid
  0,      // invalid
  &kseg0, // segment 0
  &kseg1, // segment 1
  &kseg2, // kernel segment
  &kseg2  // kernel segment
};

void r3051_init(struct r3051* processor) {
  memset(processor, 0, sizeof(struct r3051));

  processor->pc = 0xbfc00000;
}

void r3051_kill(struct r3051* processor) {
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

uint32_t r3051_fetch_byte(uint32_t address) {
  uint32_t data;

  if (cop0->registers[12] & (1 << 16)) {
    data = r3051_dcache_fetch(address);
  }
  else {
    data = bus_fetch(address);
  }

  return (int8_t) (data >> (8 * (address & 3)));
}

uint32_t r3051_fetch_half(uint32_t address) {
  if (address & 1) {
    assert(0 && "Address Exception");
  }

  uint32_t data;

  if (cop0->registers[12] & (1 << 16)) {
    data = r3051_dcache_fetch(address);
  }
  else {
    data = bus_fetch(address);
  }

  return (int16_t) (data >> (8 * (address & 2)));
}

uint32_t r3051_fetch_word(uint32_t address) {
  if (address & 3) {
    assert(0 && "Address Exception");
  }

  if (cop0->registers[12] & (1 << 16)) {
    return r3051_dcache_fetch(address);
  }
  else {
    return bus_fetch(address);
  }
}

void r3051_store_byte(uint32_t address, uint32_t data) {
  data = data & 0xff;

  if (cop0->registers[12] & (1 << 16)) {
    r3051_dcache_store(address, data);
  }
  else {
    uint32_t mask = 0xff;
    uint32_t temp = bus_fetch(address & ~3u);

    temp &= ~(mask << (8 * (address & 3)));
    temp |=  (data << (8 * (address & 3)));

    bus_store(address & ~3u, temp);
  }
}

void r3051_store_half(uint32_t address, uint32_t data) {
  if (address & 1) {
    assert(0 && "Address Exception");
  }

  data = data & 0xffff;

  if (cop0->registers[12] & (1 << 16)) {
    r3051_dcache_store(address, data);
  }
  else {
    uint32_t mask = 0xffff;
    uint32_t temp = bus_fetch(address & ~2u);

    temp &= ~(mask << (8 * (address & 2)));
    temp |=  (data << (8 * (address & 2)));

    bus_store(address & ~2u, temp);
  }
}

void r3051_store_word(uint32_t address, uint32_t data) {
  if (address & 3) {
    assert(0 && "Address Exception");
  }

  if (cop0->registers[12] & (1 << 16)) {
    r3051_dcache_store(address, data);
  }
  else {
    bus_store(address, data);
  }
}

uint32_t r3051_fetch_inst(uint32_t address) {
  if (address & 3) {
    assert(0 && "Address Exception");
  }

  struct r3051_segment* segment = segments[address >> 29];

  if (!segment) {
    assert(0 && "Access Violation");
  }

  if (segment->cached) {
    return r3051_icache_fetch(address);
  }
  else {
    return bus_fetch(address);
  }
}
