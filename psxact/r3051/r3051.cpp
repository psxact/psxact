#include "stdafx.h"
#include "r3051.h"
#include "cop0.h"
#include "..\bus\bus.h"

extern Cop0* cop0;

uint32_t address_mask[5] = { 0x00000000, 0x00000000, 0x00000001, 0x00000001, 0x00000003 };

static R3051::Segment kuseg = {
  0x00000000, // start
  0x80000000, // length
  0x40000000, // offset
  true        // cached
};

static R3051::Segment kseg0 = {
  0x80000000, // start
  0x20000000, // length
  0x00000000, // offset
  true        // cached
};

static R3051::Segment kseg1 = {
  0xa0000000, // start
  0x20000000, // length
  0x00000000, // offset
  false       // cached
};

static R3051::Segment kseg2 = {
  0xc0000000, // start
  0x40000000, // length
  0x00000000, // offset
  false       // cached
};

static R3051::Segment* segments[8] = {
  &kuseg, // user segment
  0,      // invalid
  0,      // invalid
  0,      // invalid
  &kseg0, // segment 0
  &kseg1, // segment 1
  &kseg2, // kernel segment
  &kseg2  // kernel segment
};

void R3051::r3051_init() {
  memset(this, 0, sizeof(R3051));

  this->pc = 0xbfc00000;
}

void R3051::r3051_kill() {
}

void R3051::r3051_step() {
  this->r3051_stage_wb();

  this->r3051_stage_dc();
  this->wb = this->dc;

  this->r3051_stage_ex();
  this->dc = this->ex;

  this->r3051_stage_rf();
  this->ex = this->rf;

  this->r3051_stage_ic();
  this->rf = this->ic;
}

uint32_t R3051::fetchByte(uint32_t address) {
  uint32_t data;

  if (cop0->registers[12] & (1 << 16)) {
    data = this->dcacheFetch(address);
  }
  else {
    data = bus.Fetch(address);
  }

  return (int8_t) (data >> (8 * (address & 3)));
}

uint32_t R3051::fetchHalf(uint32_t address) {
  if (address & 1) {
    assert(0 && "Address Exception");
  }

  uint32_t data;

  if (cop0->registers[12] & (1 << 16)) {
    data = this->dcacheFetch(address);
  }
  else {
    data = bus.Fetch(address);
  }

  return (int16_t) (data >> (8 * (address & 2)));
}

uint32_t R3051::fetchWord(uint32_t address) {
  if (address & 3) {
    assert(0 && "Address Exception");
  }

  if (cop0->registers[12] & (1 << 16)) {
    return this->dcacheFetch(address);
  }
  else {
    return bus.Fetch(address);
  }
}

void R3051::storeByte(uint32_t address, uint32_t data) {
  data = data & 0xff;

  if (cop0->registers[12] & (1 << 16)) {
    this->dcacheStore(address, data);
  }
  else {
    uint32_t mask = 0xff;
    uint32_t temp = bus.Fetch(address & ~3u);

    temp &= ~(mask << (8 * (address & 3)));
    temp |=  (data << (8 * (address & 3)));

    bus.Store(address & ~3u, temp);
  }
}

void R3051::storeHalf(uint32_t address, uint32_t data) {
  if (address & 1) {
    assert(0 && "Address Exception");
  }

  data = data & 0xffff;

  if (cop0->registers[12] & (1 << 16)) {
    this->dcacheStore(address, data);
  }
  else {
    uint32_t mask = 0xffff;
    uint32_t temp = bus.Fetch(address & ~2u);

    temp &= ~(mask << (8 * (address & 2)));
    temp |=  (data << (8 * (address & 2)));

    bus.Store(address & ~2u, temp);
  }
}

void R3051::storeWord(uint32_t address, uint32_t data) {
  if (address & 3) {
    assert(0 && "Address Exception");
  }

  if (cop0->registers[12] & (1 << 16)) {
    this->dcacheStore(address, data);
  }
  else {
    bus.Store(address, data);
  }
}

uint32_t R3051::fetchInst(uint32_t address) {
  if (address & 3) {
    assert(0 && "Address Exception");
  }

  R3051::Segment* segment = segments[address >> 29];

  if (!segment) {
    assert(0 && "Access Violation");
  }

  if (segment->cached) {
    return this->icacheFetch(address);
  }
  else {
    return bus.Fetch(address);
  }
}
