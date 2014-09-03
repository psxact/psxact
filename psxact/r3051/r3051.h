#include "stdafx.h"

enum r3051_datatype {
   BYTE = 0,
  UBYTE = 1,
   HALF = 2,
  UHALF = 3,
   WORD = 4
};

struct r3051_pipestage {
  uint32_t address;
  uint32_t code;
  uint32_t target;

  uint32_t si;
  uint8_t op;
  uint8_t rs;
  uint8_t rt;
  uint8_t rd;
  uint8_t sh;
  uint8_t fn;
};

struct r3051 {
  uint32_t registers[32];
  uint32_t lo;
  uint32_t hi;
  uint32_t pc;

  struct r3051_pipestage ic;
  struct r3051_pipestage rf;
  struct r3051_pipestage ex;
  struct r3051_pipestage dc;
  struct r3051_pipestage wb;
};

//
// Processor State
//

void r3051_init(struct r3051*);
void r3051_step(struct r3051*);

//
// Pipeline Stages
//

void r3051_stage_ic(struct r3051*);
void r3051_stage_rf(struct r3051*);
void r3051_stage_ex(struct r3051*);
void r3051_stage_dc(struct r3051*);
void r3051_stage_wb(struct r3051*);

//
// Co-processors
//

void r3051_cp0(struct r3051*);
void r3051_cp1(struct r3051*);
void r3051_cp2(struct r3051*);
void r3051_cp3(struct r3051*);

//
// D-Cache Functions
//

void r3051_dcache_fetch(enum r3051_datatype, uint32_t, uint32_t*);
void r3051_dcache_store(enum r3051_datatype, uint32_t, uint32_t*);

//
// I-Cache Functions
//

void r3051_icache_fetch(enum r3051_datatype, uint32_t, uint32_t*);
void r3051_icache_store(enum r3051_datatype, uint32_t, uint32_t*);
