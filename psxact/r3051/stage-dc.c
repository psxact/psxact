#include "stdafx.h"
#include "r3051.h"

static void r3051_dc_lb(struct r3051*, struct r3051_stage*);
static void r3051_dc_lbu(struct r3051*, struct r3051_stage*);
static void r3051_dc_lw(struct r3051*, struct r3051_stage*);
static void r3051_dc_sb(struct r3051*, struct r3051_stage*);
static void r3051_dc_sh(struct r3051*, struct r3051_stage*);
static void r3051_dc_sw(struct r3051*, struct r3051_stage*);

void r3051_stage_dc(struct r3051* processor) {
  struct r3051_stage* stage = &processor->dc;

  switch (stage->op) {
  case 0x00: return;
  case 0x01: return;
  case 0x02: return; // j
  case 0x03: return; // jal
  case 0x04: return; // beq
  case 0x05: return; // bne
  case 0x06: return; // blez
  case 0x07: return; // bgtz
  case 0x08: return; // addi
  case 0x09: return; // addiu
  case 0x0a: return; // slti
  case 0x0b: return; // sltiu
  case 0x0c: return; // andi
  case 0x0d: return; // ori

  case 0x0f: return; // lui
  case 0x10: return; // cop0

  case 0x20: r3051_dc_lb(processor, stage); return; // lb

  case 0x23: r3051_dc_lw(processor, stage); return; // lw
  case 0x24: r3051_dc_lbu(processor, stage); return; // lbu

  case 0x28: r3051_dc_sb(processor, stage); return; // sb
  case 0x29: r3051_dc_sh(processor, stage); return; // sh

  case 0x2b: r3051_dc_sw(processor, stage); return; // sw
  }

  assert(0 && "Unimplemented instruction");
}

static void r3051_dc_lb(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rt] = r3051_fetch_byte(stage->target);
}

static void r3051_dc_lbu(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rt] = r3051_fetch_byte(stage->target) & 0xff;
}

static void r3051_dc_lw(struct r3051* processor, struct r3051_stage* stage) {
  processor->registers[stage->rt] = r3051_fetch_word(stage->target);
}

static void r3051_dc_sb(struct r3051* processor, struct r3051_stage* stage) {
  r3051_store_byte(stage->target, processor->registers[stage->rt]);
}

static void r3051_dc_sh(struct r3051* processor, struct r3051_stage* stage) {
  r3051_store_half(stage->target, processor->registers[stage->rt]);
}

static void r3051_dc_sw(struct r3051* processor, struct r3051_stage* stage) {
  r3051_store_word(stage->target, processor->registers[stage->rt]);
}
