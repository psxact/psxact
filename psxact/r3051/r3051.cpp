#include "stdafx.h"
#include "r3051.h"
#include "cop0.h"
#include "..\bus\bus.h"

static R3051::Segment kuseg(0x00000000, 0x80000000, 0x40000000, true);
static R3051::Segment kseg0(0x80000000, 0x20000000, 0x00000000, true);
static R3051::Segment kseg1(0xa0000000, 0x20000000, 0x00000000, false);
static R3051::Segment kseg2(0xc0000000, 0x40000000, 0x00000000, false);

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

R3051::R3051(void) {
  memset(this, 0, sizeof(R3051));

  this->pc = 0xbfc00000;
  this->cop0 = new Cop0();
}

R3051::~R3051(void) {
  delete this->cop0;
}

void R3051::AttachBus(Bus* bus) {
  this->bus = bus;
}

void R3051::Step() {
  this->StageWb();
  this->StageDc();
  this->StageEx();
  this->StageRf();
  this->StageIc();

  this->wb = this->dc;
  this->dc = this->ex;
  this->ex = this->rf;
  this->rf = this->ic;
}

uint32_t R3051::FetchByte(uint32_t address) {
  uint32_t data;

  if (cop0->FetchSr(12) & (1 << 16)) {
    data = this->DCacheFetch(address);
  }
  else {
    data = bus->Fetch(address);
  }

  return (int8_t) (data >> (8 * (address & 3)));
}

uint32_t R3051::FetchHalf(uint32_t address) {
  if (address & 1) {
    assert(0 && "Address Exception");
  }

  uint32_t data;

  if (cop0->FetchSr(12) & (1 << 16)) {
    data = this->DCacheFetch(address);
  }
  else {
    data = bus->Fetch(address);
  }

  return (int16_t) (data >> (8 * (address & 2)));
}

uint32_t R3051::FetchWord(uint32_t address) {
  if (address & 3) {
    assert(0 && "Address Exception");
  }

  if (cop0->FetchSr(12) & (1 << 16)) {
    return this->DCacheFetch(address);
  }
  else {
    return bus->Fetch(address);
  }
}

void R3051::StoreByte(uint32_t address, uint32_t data) {
  data = data & 0xff;

  if (cop0->FetchSr(12) & (1 << 16)) {
    this->DCacheStore(address, data);
  }
  else {
    uint32_t mask = 0xff;
    uint32_t temp = bus->Fetch(address & ~3u);

    temp &= ~(mask << (8 * (address & 3)));
    temp |=  (data << (8 * (address & 3)));

    bus->Store(address & ~3u, temp);
  }
}

void R3051::StoreHalf(uint32_t address, uint32_t data) {
  if (address & 1) {
    assert(0 && "Address Exception");
  }

  data = data & 0xffff;

  if (cop0->FetchSr(12) & (1 << 16)) {
    this->DCacheStore(address, data);
  }
  else {
    uint32_t mask = 0xffff;
    uint32_t temp = bus->Fetch(address & ~2u);

    temp &= ~(mask << (8 * (address & 2)));
    temp |=  (data << (8 * (address & 2)));

    bus->Store(address & ~2u, temp);
  }
}

void R3051::StoreWord(uint32_t address, uint32_t data) {
  if (address & 3) {
    assert(0 && "Address Exception");
  }

  if (cop0->FetchSr(12) & (1 << 16)) {
    this->DCacheStore(address, data);
  }
  else {
    bus->Store(address, data);
  }
}

uint32_t R3051::FetchInst(uint32_t address) {
  if (address & 3) {
    assert(0 && "Address Exception");
  }

  R3051::Segment* segment = segments[address >> 29];

  if (!segment) {
    assert(0 && "Access Violation");
  }

  if (segment->cached) {
    return this->ICacheFetch(address);
  }
  else {
    return bus->Fetch(address);
  }
}

R3051::Segment::Segment(uint32_t start, uint32_t length, uint32_t offset, bool cached) {
  this->start = start;
  this->length = length;
  this->offset = offset;
  this->cached = cached;
}
