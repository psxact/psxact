#include "stdafx.h"
#include "bus.h"

#define IN_RANGE(a, b) ((address & ~((a) ^ (b))) == (a))

uint8_t* bios;
uint8_t* disk;
uint8_t  wram[0x200000];

//
// Read data from the bus
//

int bus_fetch(uint32_t address, uint32_t* data) {
  if (
    IN_RANGE(0x00000000, 0x007fffff) || // KUSEG
    IN_RANGE(0x80000000, 0x807fffff) || // KSEG0
    IN_RANGE(0xa0000000, 0xa07fffff)) { // KSEG1
    *data = *((uint32_t*)(wram + (address & 0x1ffffc)));
    return BUS_OK;
  }

  if (
    IN_RANGE(0x1f000000, 0x1f7fffff) || // KUSEG
    IN_RANGE(0x9f000000, 0x9f7fffff) || // KSEG0
    IN_RANGE(0xbf000000, 0xbf7fffff)) { // KSEG1
    *data = 0x00000000;
    printf("[W] EXP1: [%08x]\n", address);
    return BUS_OK;
  }

  if (
    IN_RANGE(0x1f800000, 0x1f8003ff) || // KUSEG
    IN_RANGE(0x9f800000, 0x9f8003ff)) { // KSEG0
    *data = 0x00000000;
    printf("[W] Scratchpad: [%08x]\n", address);
    return BUS_OK;
  }

  if (
    IN_RANGE(0x1f801000, 0x1f801fff) || // KUSEG
    IN_RANGE(0x9f801000, 0x9f801fff) || // KSEG0
    IN_RANGE(0xbf801000, 0xbf801fff)) { // KSEG1
    *data = 0x00000000;
    printf("[W] I/O: [%08x]\n", address);
    return BUS_OK;
  }

  if (
    IN_RANGE(0x1f802000, 0x1f802fff) || // KUSEG
    IN_RANGE(0x9f802000, 0x9f802fff) || // KSEG0
    IN_RANGE(0xbf802000, 0xbf802fff)) { // KSEG1
    *data = 0x00000000;
    printf("[W] EXP2: [%08x]\n", address);
    return BUS_OK;
  }

  if (
    IN_RANGE(0x1fa00000, 0x1fbfffff) || // KUSEG
    IN_RANGE(0x9fa00000, 0x9fbfffff) || // KSEG0
    IN_RANGE(0xbfa00000, 0xbfbfffff)) { // KSEG1
    *data = 0x00000000;
    printf("[W] EXP3: [%08x]\n", address);
    return BUS_OK;
  }

  if (
    IN_RANGE(0x1fc00000, 0x1fc7ffff) || // KUSEG
    IN_RANGE(0x9fc00000, 0x9fc7ffff) || // KSEG0
    IN_RANGE(0xbfc00000, 0xbfc7ffff)) { // KSEG1
    *data = *((uint32_t*)(bios + (address & 0x7fffc)));
    return BUS_OK;
  }

  if (address == 0xfffe0130) {
    *data = 0x00000000;
    printf("Cache Control Register: [%08x]\n", address);
    return BUS_OK;
  }

  printf("Unknown Read: [%08x]\n", address);
  return BUS_ERROR;
}

//
// Write data to the bus
//

int bus_store(uint32_t address, uint32_t* data) {
  if (
    IN_RANGE(0x00000000, 0x001fffff) || // KUSEG
    IN_RANGE(0x80000000, 0x801fffff) || // KSEG0
    IN_RANGE(0xa0000000, 0xa01fffff)) { // KSEG1
    *((uint32_t*)(wram + (address & 0x1ffffc))) = *data;
    return BUS_OK;
  }

  if (
    IN_RANGE(0x1f000000, 0x1f7fffff) || // KUSEG
    IN_RANGE(0x9f000000, 0x9f7fffff) || // KSEG0
    IN_RANGE(0xbf000000, 0xbf7fffff)) { // KSEG1
    printf("[W] EXP1: [%08x] <= %08x\n", address, *data);
    return BUS_OK;
  }

  if (
    IN_RANGE(0x1f800000, 0x1f8003ff) || // KUSEG
    IN_RANGE(0x9f800000, 0x9f8003ff)) { // KSEG0
    printf("[W] Scratchpad: [%08x] <= %08x\n", address, *data);
    return BUS_OK;
  }

  if (
    IN_RANGE(0x1f801000, 0x1f801fff) || // KUSEG
    IN_RANGE(0x9f801000, 0x9f801fff) || // KSEG0
    IN_RANGE(0xbf801000, 0xbf801fff)) { // KSEG1
    printf("[W] I/O: [%08x] <= %08x\n", address, *data);
    return BUS_OK;
  }

  if (
    IN_RANGE(0x1f802000, 0x1f802fff) || // KUSEG
    IN_RANGE(0x9f802000, 0x9f802fff) || // KSEG0
    IN_RANGE(0xbf802000, 0xbf802fff)) { // KSEG1
    printf("[W] EXP2: [%08x] <= %08x\n", address, *data);
    return BUS_OK;
  }

  if (
    IN_RANGE(0x1fa00000, 0x1fbfffff) || // KUSEG
    IN_RANGE(0x9fa00000, 0x9fbfffff) || // KSEG0
    IN_RANGE(0xbfa00000, 0xbfbfffff)) { // KSEG1
    printf("[W] EXP3: [%08x] <= %08x\n", address, *data);
    return BUS_OK;
  }

  if (address == 0xfffe0130) {
    printf("Cache Control Register: [%08x] <= %08x\n", address, *data);
    return BUS_OK;
  }

  printf("Unknown Write: [%08x] <= %08x\n", address, *data);
  return BUS_ERROR;
}
