#include "cpu/segment.hpp"

using namespace psx::cpu;

static const segment segments[8] = {
  segment::kuseg,
  segment::kuseg,
  segment::kuseg,
  segment::kuseg,
  segment::kseg0,
  segment::kseg1,
  segment::kseg2,
  segment::kseg2
};

segment psx::cpu::get_segment(uint32_t address) {
  return segments[address >> 29];
}

static const uint32_t segment_masks[8] = {
  0x7fff'ffff,
  0x7fff'ffff,
  0x7fff'ffff,
  0x7fff'ffff,
  0x1fff'ffff,
  0x1fff'ffff,
  0xffff'ffff,
  0xffff'ffff
};

uint32_t psx::cpu::get_segment_mask(uint32_t address) {
  return segment_masks[address >> 29];
}

bool psx::cpu::is_segment_cached(uint32_t address) {
  return get_segment(address) < segment::kseg1;
}
