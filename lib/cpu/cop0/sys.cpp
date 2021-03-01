#include "cpu/cop0/sys.hpp"

#include "util/panic.hpp"

using namespace psx::cpu::cop0;
using namespace psx::util;

sys::sys() : logger("sys", false) {
}

void sys::run(uint32_t n) {
  if (n == 0x10) {
    return pop_flags();
  }
}

uint32_t sys::read_ccr(uint32_t) {
  PANIC("Tried to read unimplemented COP0 register, should trigger RI exception.");
  return 0;
}

void sys::write_ccr(uint32_t, uint32_t) {
  PANIC("Tried to write unimplemented COP0 register, should trigger RI exception.");
}

uint32_t sys::read_gpr(uint32_t n) {
  switch (n) {
    case 0x3: // BPC (Read-write)
      return bpc;

    case 0x5: // BDA (Read-write)
      return bda;

    case 0x6: // TAR (Read-only)
      return tar;

    case 0x7: // DCIC (Read-write)
      return dcic;

    case 0x8: // BadVaddr (Read-only)
      return bad_vaddr;

    case 0x9: // BDAM (Read-write)
      return bdam;

    case 0xb: // BPCM (Read-write)
      return bpcm;

    case 0xc: // SR (Read-write)
      return sr;

    case 0xd: // CAUSE (Read-only)
      return cause;

    case 0xe: // EPC (Read-only)
      return epc;

    case 0xf: // PRID (Read-only)
      return 0x0000'0002;

    default:
      PANIC("Tried to read unimplemented COP0 register, should trigger RI exception.");
      return 0;
  }
}

void sys::write_gpr(uint32_t n, uint32_t value) {
  switch (n) {
    case 0x3: bpc = value; break;
    case 0x5: bda = value; break;
    case 0x6: break;
    case 0x7: dcic = value; break;
    case 0x8: break;
    case 0x9: bdam = value; break;
    case 0xb: bpcm = value; break;
    case 0xc: sr = value; break;
    case 0xd: // -0-- 0000 0000 0000 ---- --xx 0--- --00
      cause = (cause & 0xb000fc7c) | (value & 0x00000300);
      break;

    case 0xe: break;
    case 0xf: break;
  }
}

bool sys::get_bev() {
  return (sr & (1 << 22)) != 0;
}

void sys::put_tar(uint32_t val) {
  tar = val;
}

void sys::put_cause_excode(exception val) {
  cause = (cause & ~0x0000007c) | (int(val) << 2);
}

void sys::put_cause_ip(bool val) {
  cause = (cause & ~0x00000400) | (int(val) << 10);
}

void sys::put_cause_ce(int32_t val) {
  cause = (cause & ~0x30000000) | ((val & 3) << 28);
}

void sys::put_cause_bt(bool val) {
  cause = (cause & ~0x40000000) | (int(val) << 30);
}

void sys::put_cause_bd(bool val) {
  cause = (cause & ~0x80000000) | (int(val) << 31);
}

void sys::put_epc(uint32_t val) {
  epc = val;
}

void sys::push_flags() {
  sr = (sr & ~0x0000003f) | ((sr << 2) & 0x3f);
}

void sys::pop_flags() {
  sr = (sr & ~0x0000000f) | ((sr >> 2) & 0x0f);
}
