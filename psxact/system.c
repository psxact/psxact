#include "stdafx.h"
#include "r3051\r3051.h"
#include "r3051\cp0.h"

struct r3051* r3051;
struct r3051_cop0* cop0;

extern uint8_t* bios;
extern uint8_t* disk;

void psx_init(void) {
  r3051 = (struct r3051*) malloc(sizeof(struct r3051));
  memset(r3051, 0, sizeof(struct r3051));

  r3051_init(r3051);

  cop0 = (struct r3051_cop0*) malloc(sizeof(struct r3051_cop0));
  memset(cop0, 0, sizeof(struct r3051_cop0));

  r3051_cop0_init(cop0);
}

void psx_kill(void) {
}

void psx_step(void) {
  r3051_step(r3051);
}
