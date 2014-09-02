#include "stdafx.h"
#include "r3051.h"

//
// RF Stage
//

#define DECODE_JT() ((rf->code >>  0) & 0x3ffffff)
#define DECODE_SI() ((rf->code >>  0) & 0xffff)
#define DECODE_SH() ((rf->code >>  6) & 31)
#define DECODE_RD() ((rf->code >> 11) & 31)
#define DECODE_RT() ((rf->code >> 16) & 31)
#define DECODE_RS() ((rf->code >> 21) & 31)

void r3051_stage_rf_itype(struct r3051* processor) {
  struct r3051_pipestage *rf = &processor->rf;

  rf->si = DECODE_SI();
  rf->rt = DECODE_RT();
  rf->rs = DECODE_RS();
}

void r3051_stage_rf_jtype(struct r3051* processor) {
  struct r3051_pipestage* rf = &processor->rf;

  rf->si = DECODE_JT();
}

void r3051_stage_rf_rtype(struct r3051* processor) {
  struct r3051_pipestage *rf = &processor->rf;

  rf->sh = DECODE_SH();
  rf->rd = DECODE_RD();
  rf->rt = DECODE_RT();
  rf->rs = DECODE_RS();
}

void r3051_stage_rf_00(struct r3051* processor) {
  struct r3051_pipestage *rf = &processor->rf;

  switch ((rf->code >> 0) & 63) {
  case 0x00: // sll rd,rt,sa
  case 0x08: // jr rs
  case 0x21: // addu rd,rs,rt
  case 0x25: // or rd,rs,rt
  case 0x2b: // sltu rd,rs,rt
    return r3051_stage_rf_rtype(processor);
  }

  assert(0 && "Unimplemented instruction");
}

void r3051_stage_rf(struct r3051* processor) {
  struct r3051_pipestage *rf = &processor->rf;

  switch ((rf->code >> 26) & 63) {
  case 0x00: return r3051_stage_rf_00(processor);
  case 0x02:
  case 0x03: return r3051_stage_rf_jtype(processor);
  case 0x05: return r3051_stage_rf_itype(processor);
  case 0x08:
  case 0x09:
  case 0x0c: // andi rt,rs,immediate
  case 0x0d:
  case 0x0f:
    return r3051_stage_rf_itype(processor);

  case 0x10: /* cp0 */
  case 0x11: /* cp1 */
  case 0x12: /* cp2 */
  case 0x13: /* cp3 */
    return r3051_stage_rf_rtype(processor);
    
  case 0x20: // lb rt,offset(rs)
  case 0x23: // lw rt,offset(rs)
  case 0x28: // sb rt,offset(rs)
  case 0x29: // sh rt,offset(rs)
  case 0x2b: // sw rt,offset(rs)
    return r3051_stage_rf_itype(processor);
  }

  assert(0 && "Unimplemented instruction");
}
