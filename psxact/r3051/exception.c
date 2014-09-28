#include "stdafx.h"
#include "r3051.h"
#include "cop0.h"

void r3051_syscall(struct r3051_cop0* cop0, struct r3051* processor) {
	uint32_t cause = cop0->registers[13];

	// ExCode = 8

	cause &= ~0x7c;
	cause |= 0x08 << 2;

	cop0->registers[13] = cause;
	cop0->registers[14] = processor->ex.address;

	processor->pc = 0x80000080;
	processor->rf.code = 0;
}
