#include "stdafx.h"
#include "bus.h"

uint8_t* bios;
uint8_t* disk;
uint8_t  wram[0x200000];

#define Access(memory, address) *((uint32_t*)(memory + (address)))
#define BusLog(format, ...) //printf(format, __VA_ARGS__)
#define Within(a, b) ((address & ~((a) ^ (b))) == (a))

//
// Read data from the bus

uint32_t bus_fetch(uint32_t address) {
  if (address == 0xfffe0130) {
    BusLog("[R] Cache Control Register\n");
    return 0x00000000;
  }

  address = address & ~0xe0000000;
  
  if (Within(0x00000000, 0x007fffff)) {
    return Access(wram, address & 0x1ffffc);
  }

  if (Within(0x1f000000, 0x1f7fffff)) {
    BusLog("[R] EXP1 $%08x\n", address);
    return 0x00000000;
  }

  if (Within(0x1f800000, 0x1f8003ff)) {
    BusLog("[R] DCache $%08x\n", address);
    return 0x00000000;
  }

  if (Within(0x1f801000, 0x1f801fff)) {
    BusLog("[R] I/O $%08x\n", address);
    return 0x00000000;
  }

  if (Within(0x1f802000, 0x1f802fff)) {
    BusLog("[R] EXP2 $%08x\n", address);
    return 0x00000000;
  }

  if (Within(0x1fa00000, 0x1fbfffff)) {
    BusLog("[R] EXP3 $%08x\n", address);
    return 0x00000000;
  }

  if (Within(0x1fc00000, 0x1fc7ffff)) {
    return Access(bios, address & 0x7fffc);
  }

  printf("Unknown Read: [%08x]\n", address);
  return 0;
}

//
// Write data to the bus

void bus_store(uint32_t address, uint32_t data) {
  if (address == 0xfffe0130) {
    BusLog("[W] Cache Control Register <= $%08x\n", data);
    return;
  }

  address = address & ~0xe0000000;

  if (Within(0x00000000, 0x001fffff)) {
    Access(wram, address & 0x1ffffc) = data;
    return;
  }

  if (Within(0x1f000000, 0x1f7fffff)) {
    BusLog("[W] EXP1 $%08x <= $%08x\n", address, data);
    return;
  }

  if (Within(0x1f800000, 0x1f8003ff)) {
    BusLog("[W] DCache $%08x <= $%08x\n", address, data);
    return;
  }

  if (Within(0x1f801000, 0x1f801fff)) {
    BusLog("[W] I/O $%08x <= $%08x\n", address, data);
    return;
  }

  if (Within(0x1f802000, 0x1f802fff)) {
    BusLog("[W] EXP2 $%08x <= $%08x\n", address, data);
    return;
  }

  if (Within(0x1fa00000, 0x1fbfffff)) {
    BusLog("[W] EXP3 $%08x <= $%08x\n", address, data);
    return;
  }

  if (Within(0x1fc00000, 0x1fc7ffff)) {
    BusLog("[W] BIOS $%08x <= $%08x\n", address, data);
    return;
  }

  printf("Unknown Write: [%08x] <= %08x\n", address, data);
}
