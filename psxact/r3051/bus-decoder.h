#include "stdafx.h"

// kuseg: $00000000-$7fffffff
// kseg0: $80000000-$9fffffff
// kseg1: $a0000000-$bfffffff
// kseg2: $c0000000-$ffffffff

void r3051_bus_read(int, uint32_t, uint32_t*);
void r3051_bus_write(int, uint32_t, uint32_t);
