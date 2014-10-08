#include "stdafx.h"
#include "system.h"
#include "r3051\r3051.h"
#include "bus\bus.h"
#include "gpu\gpu.h"

PlayStation::PlayStation(void) {
  cpu = new R3051();
  gpu = new Gpu();
  bus = new Bus(gpu);

  cpu->AttachBus(bus);
}

PlayStation::~PlayStation(void) {
  delete cpu;
  delete gpu;
  delete bus;
}

void PlayStation::Step(void) {
  cpu->Step();
}

bool PlayStation::LoadBiosImage(const char* fileName) {
  return Util::LoadFile(fileName, bus->GetBios());
}

bool PlayStation::LoadDiskImage(const char* fileName) {
  return Util::LoadFile(fileName, bus->GetDisk());
}
