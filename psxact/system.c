#include "stdafx.h"
#include "r3051\r3051.h"

struct r3051* r3051;

extern uint8_t* bios;
extern uint8_t* disk;

void psx_init(void) {
  r3051 = (struct r3051*) malloc(sizeof(struct r3051));
  memset(r3051, 0, sizeof(struct r3051));

  r3051_init(r3051);
}

void psx_kill(void) {
}

void psx_step(void) {
  r3051_step(r3051);
}
