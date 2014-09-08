#include "stdafx.h"
#include "bus.h"

#define IN_RANGE(a, b) ((address & ~((a) ^ (b))) == (a))

uint8_t* bios;
uint8_t* disk;
uint8_t  wram[0x200000];

#define access_32(memory, address) *((uint32_t*)(memory + (address)))

//
// Read data from the bus
//

uint32_t bus_fetch(uint32_t address) {
  if (address == 0xfffe0130) {
    printf("[R] Cache Control Register\n");
    return 0x00000000;
  }

  address = address & ~0xe0000000;
  
  if (IN_RANGE(0x00000000, 0x007fffff)) {
    return access_32(wram, address & 0x1ffffc);
  }

  if (IN_RANGE(0x1f000000, 0x1f7fffff)) {
    printf("[R] EXP1 $%08x\n", address);
    return 0x00000000;
  }

  if (IN_RANGE(0x1f800000, 0x1f8003ff)) {
    printf("[R] DCache $%08x\n", address);
    return 0x00000000;
  }

  if (IN_RANGE(0x1f801000, 0x1f801fff)) {
    printf("[R] I/O $%08x\n", address);
    return 0x00000000;
  }

  if (IN_RANGE(0x1f802000, 0x1f802fff)) {
    printf("[R] EXP2 $%08x\n", address);
    return 0x00000000;
  }

  if (IN_RANGE(0x1fa00000, 0x1fbfffff)) {
    printf("[R] EXP3 $%08x\n", address);
    return 0x00000000;
  }

  if (IN_RANGE(0x1fc00000, 0x1fc7ffff)) {
    return access_32(bios, address & 0x7fffc);
  }

  printf("Unknown Read: [%08x]\n", address);
  return 0;
}

//
// Write data to the bus
//

void bus_store(uint32_t address, uint32_t data) {
  if (address == 0xfffe0130) {
    printf("[W] Cache Control Register <= $%08x\n", data);
    return;
  }

  address = address & ~0xe0000000;

  if (IN_RANGE(0x00000000, 0x001fffff)) {
    access_32(wram, address & 0x1ffffc) = data;
    return;
  }

  if (IN_RANGE(0x1f000000, 0x1f7fffff)) {
    printf("[W] EXP1 $%08x <= $%08x\n", address, data);
    return;
  }

  if (IN_RANGE(0x1f800000, 0x1f8003ff)) {
    printf("[W] DCache $%08x <= $%08x\n", address, data);
    return;
  }

  if (IN_RANGE(0x1f801000, 0x1f801fff)) {
    printf("[W] I/O $%08x <= $%08x\n", address, data);
    return;
  }

  if (IN_RANGE(0x1f802000, 0x1f802fff)) {
    printf("[W] EXP2 $%08x <= $%08x\n", address, data);
    return;
  }

  if (IN_RANGE(0x1fa00000, 0x1fbfffff)) {
    printf("[W] EXP3 $%08x <= $%08x\n", address, data);
    return;
  }

  printf("Unknown Write: [%08x] <= %08x\n", address, data);
}
