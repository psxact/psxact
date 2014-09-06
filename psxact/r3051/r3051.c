#include "stdafx.h"
#include "r3051.h"
#include "cp0.h"
#include "..\bus\bus.h"

struct r3051_dcache dcache;
struct r3051_icache icache;

struct r3051_segment kuseg = {
  0x00000000, /* start  */
  0x80000000, /* length */
  0x40000000, /* offset */
  true        /* cached */
};

struct r3051_segment kseg0 = {
  0x80000000, /* start  */
  0x20000000, /* length */
  0x00000000, /* offset */
  true        /* cached */
};

struct r3051_segment kseg1 = {
  0xa0000000, /* start  */
  0x20000000, /* length */
  0x00000000, /* offset */
  false       /* cached */
};

struct r3051_segment kseg2 = {
  0xc0000000, /* start  */
  0x40000000, /* length */
  0x00000000, /* offset */
  false       /* cached */
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

void r3051_dcache_fetch(uint32_t address, uint32_t* data) {
  *data = dcache.lines[(address >> 2) & 0x3ff][0];
}

void r3051_dcache_store(uint32_t address, uint32_t* data) {
  dcache.lines[(address >> 2) & 0x3ff][0] = *data;
}

void r3051_icache_fetch(uint32_t address, uint32_t* data) {
  uint32_t index = (address >> 4) & 0x3ff;
  uint32_t *line = icache.lines[index];

  if (icache.valid[index] == 0 || icache.tag[index] != (address & 0xfffff000)) {
    printf("ICache Fill: $%08x\n", address);
    address = (address & ~0xf);

    bus_fetch(address +  0, &line[0]);
    bus_fetch(address +  4, &line[1]);
    bus_fetch(address +  8, &line[2]);
    bus_fetch(address + 12, &line[3]);

    icache.valid[index] = 1;
    icache.tag[index] = address & 0xfffff000;
  }

  *data = line[(address >> 2) & 3];
}

void r3051_icache_store(uint32_t address, uint32_t* data) {
  uint32_t index = (address >> 4) & 0x3ff;
  uint32_t *line = icache.lines[index];

  printf("[W] ICache $%08x <= $%08x\n", address, *data);
}
void r3051_fetch_data(enum r3051_datatype datatype, uint32_t address, uint32_t* data) {
  if (address & address_mask[datatype]) {
    printf("DCache Address Exception - $%08x\n", address);
    return;
  }

  struct r3051_segment *segment = segments[address >> 29];

  if (!segment) {
    printf("DCache Access Violation - $%08x\n", address);
    return;
  }

  address += segment->offset;

  if (segment->cached) {
    r3051_dcache_fetch(address, data);
  }
  else {
    bus_fetch(address, data);
  }
}

void r3051_store_data(enum r3051_datatype datatype, uint32_t address, uint32_t* data) {
  if (address & address_mask[datatype]) {
    printf("DCache Address Exception - $%08x\n", address);
    return;
  }

  struct r3051_segment *segment = segments[address >> 29];

  if (!segment) {
    printf("DCache Access Violation - $%08x\n", address);
    return;
  }

  if (cp0->registers[12] & (1 << 16)) { // IsC
    if (cp0->registers[12] & (1 << 17)) { // SwC
      r3051_icache_store(address, data);
    }
    else {
      r3051_dcache_store(address, data);
    }
  }
  else {
    if (segment->cached) {
      r3051_dcache_store(address, data);
    }

    // always write to memory, even if cached
    bus_store(address, data);
  }
}

void r3051_fetch_inst(enum r3051_datatype datatype, uint32_t address, uint32_t* data) {
  if (address & address_mask[datatype]) {
    printf("ICache Address Exception - $%08x\n", address);
    return;
  }

  struct r3051_segment *segment = segments[address >> 29];

  if (!segment) {
    printf("ICache Access Violation - $%08x\n", address);
    return;
  }

  address += segment->offset;

  if (segment->cached) {
    r3051_icache_fetch(address, data);
  }
  else {
    bus_fetch(address, data);
  }
}
