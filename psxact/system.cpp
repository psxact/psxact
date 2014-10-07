#include "stdafx.h"
#include "system.h"
#include "r3051\r3051.h"
#include "r3051\cop0.h"

R3051* r3051;
Cop0* cop0;

extern uint8_t* bios;
extern uint8_t* disk;

void Psx::init(void) {
  r3051 = new R3051();

  cop0 = (Cop0*) malloc(sizeof(Cop0));
  cop0->init();
}

void Psx::kill(void) {
  r3051->~R3051();
  cop0->kill();
}

void Psx::step(void) {
  r3051->r3051_step();
}
