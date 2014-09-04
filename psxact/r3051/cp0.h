#include "stdafx.h"

struct r3051_cp0 {
  uint32_t registers[16];
};

void r3051_cp0_init(struct r3051_cp0*);
void r3051_cp0_kill(struct r3051_cp0*);

void r3051_cp0_fetch(struct r3051_cp0*, uint32_t, uint32_t*);
void r3051_cp0_store(struct r3051_cp0*, uint32_t, uint32_t*);
