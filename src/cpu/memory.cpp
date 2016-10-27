#include "cpu_core.hpp"
#include "../bus.hpp"

struct segment_t {
  uint32_t mask;
  bool cached;
};

static segment_t segments[8] = {
  { 0x7fffffff, true  }, // kuseg ($0000_0000 - $7fff_ffff)
  { 0x7fffffff, true  }, // 
  { 0x7fffffff, true  }, // 
  { 0x7fffffff, true  }, // 
  { 0x1fffffff, false }, // kseg0 ($8000_0000 - $9fff_ffff)
  { 0x1fffffff, false }, // kseg1 ($a000_0000 - $bfff_ffff)
  { 0x3fffffff, false }, // kseg2 ($c000_0000 - $ffff_ffff)
  { 0x3fffffff, false }  // 
};

static inline segment_t *get_segment(uint32_t address) {
  return &segments[address >> 29];
}

uint32_t cpu::read_code() {
  auto segment = get_segment(state.registers.pc);
  auto address = state.registers.pc & segment->mask;

  state.registers.pc = state.registers.next_pc;
  state.registers.next_pc += 4;

  // todo: read i-cache
  // if (segment->cached) {
  // }

  return bus::read(WORD, address);
}

uint32_t cpu::read_data(int size, uint32_t address) {
  if (state.cop0.registers[12] & (1 << 16)) {
    return 0; // isc=1
  }

  auto segment = get_segment(address);
  address = address & segment->mask;

  // todo: read d-cache
  // if (segment->cached) {
  // }

  return bus::read(size, address);
}

void cpu::write_data(int size, uint32_t address, uint32_t data) {
  if (state.cop0.registers[12] & (1 << 16)) {
    return; // isc=1
  }

  if (address == 0xfffe0130) {
    return; // cache control
  }

  auto segment = get_segment(address);
  address = address & segment->mask;

  // todo: write d-cache
  // if (segment->cached) {
  // }

  return bus::write(size, address, data);
}
