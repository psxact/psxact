#include "stdafx.h"
#include "r3051.h"

//
// RF Stage
//

#define DECODE_JT() ((stage->code >>  0) & 0x3ffffff)
#define DECODE_SI() ((stage->code >>  0) & 0xffff)
#define DECODE_FN() ((stage->code >>  0) & 63)
#define DECODE_SH() ((stage->code >>  6) & 31)
#define DECODE_RD() ((stage->code >> 11) & 31)
#define DECODE_RT() ((stage->code >> 16) & 31)
#define DECODE_RS() ((stage->code >> 21) & 31)
#define DECODE_OP() ((stage->code >> 26) & 63)

void r3051_stage_rf_itype(struct r3051* processor, struct r3051_pipestage* stage) {
  stage->si = DECODE_SI();
  stage->rt = DECODE_RT();
  stage->rs = DECODE_RS();
}

void r3051_stage_rf_jtype(struct r3051* processor, struct r3051_pipestage* stage) {
  stage->si = DECODE_JT();
}

void r3051_stage_rf_rtype(struct r3051* processor, struct r3051_pipestage* stage) {
  stage->sh = DECODE_SH();
  stage->rd = DECODE_RD();
  stage->rt = DECODE_RT();
  stage->rs = DECODE_RS();
}

void r3051_stage_rf_00(struct r3051* processor, struct r3051_pipestage* stage) {
  switch (stage->fn = DECODE_FN()) {
  case 0x00: // sll rd,rt,sa
  case 0x08: // jr rs
  case 0x09: // jalr rd,rs
  case 0x20: // add rd,rs,rt
  case 0x21: // addu rd,rs,rt
  case 0x24: // and rd,rs,rt
  case 0x25: // or rd,rs,rt
  case 0x2b: // sltu rd,rs,rt
    r3051_stage_rf_rtype(processor, stage);
    return;
  }

  assert(0 && "Unimplemented instruction");
}

void r3051_stage_rf(struct r3051* processor) {
  struct r3051_pipestage *stage = &processor->rf;

  switch (stage->op = DECODE_OP()) {
  case 0x00: r3051_stage_rf_00(processor, stage); return;
  case 0x02:
  case 0x03: r3051_stage_rf_jtype(processor, stage); return;
  case 0x04: // beq rs,rt,immediate
  case 0x05: // bne rs,rt,immediate
  case 0x06: // blez rs,rt,immediate
  case 0x07: // bgtz rs,rt,immediate
    r3051_stage_rf_itype(processor, stage); return;

  case 0x08:
  case 0x09:
  case 0x0c: // andi rt,rs,immediate
  case 0x0d:
  case 0x0f:
    r3051_stage_rf_itype(processor, stage);
    return;

  case 0x10: /* cp0 */
  case 0x11: /* cp1 */
  case 0x12: /* cp2 */
  case 0x13: /* cp3 */
    r3051_stage_rf_rtype(processor, stage);
    return;
    
  case 0x20: // lb rt,offset(rs)
  case 0x23: // lw rt,offset(rs)
  case 0x24: // lbu rt,offset(rs)
  case 0x28: // sb rt,offset(rs)
  case 0x29: // sh rt,offset(rs)
  case 0x2b: // sw rt,offset(rs)
    r3051_stage_rf_itype(processor, stage);
    return;
  }

  assert(0 && "Unimplemented instruction");
}
