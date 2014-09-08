#include "stdafx.h"

struct r3051_cop0 {
  uint32_t registers[16];
};

void r3051_cop0_init(struct r3051_cop0*);
void r3051_cop0_kill(struct r3051_cop0*);

uint32_t r3051_cop0_fetch_sr(struct r3051_cop0*, uint32_t);
void r3051_cop0_store_sr(struct r3051_cop0*, uint32_t, uint32_t);

uint32_t r3051_cop0_fetch_cr(struct r3051_cop0*, uint32_t);
void r3051_cop0_store_cr(struct r3051_cop0*, uint32_t, uint32_t);
