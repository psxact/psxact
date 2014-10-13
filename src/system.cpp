#include "stdafx.hpp"
#include "system.hpp"
#include "r3051/r3051.hpp"
#include "bus/bus.hpp"
#include "gpu/gpu.hpp"

PlayStation::PlayStation(void) {
  cpu = new R3051();
  gpu = new Gpu();
  spu = new Spu();
  bus = new Bus(cpu, gpu, spu);

  cpu->AttachBus(bus);
  gpu->AttachBus(bus);
  spu->AttachBus(bus);
}

PlayStation::~PlayStation(void) {
  delete bus;
  delete cpu;
  delete gpu;
  delete spu;
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
