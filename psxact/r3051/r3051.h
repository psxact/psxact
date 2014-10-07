#pragma once

#include "stdafx.h"
#include "../bus/bus.h"

class R3051 {
public:
  struct Segment {
    uint32_t start;
    uint32_t length;
    uint32_t offset;
    bool cached;
  };

  struct Stage {
    uint32_t address;
    uint32_t code;
    uint32_t target;

    uint32_t nn;
    uint8_t op;
    uint8_t rs;
    uint8_t rt;
    uint8_t rd;
    uint8_t fn;
  };

  uint32_t registers[32];
  uint32_t lo;
  uint32_t hi;
  uint32_t pc;

  Bus bus;

  Stage ic;
  Stage rf;
  Stage ex;
  Stage dc;
  Stage wb;

  //
  // Processor State

  R3051(void);
  ~R3051(void);

  void r3051_step(void);

  //
  // Pipeline Stages

  void r3051_stage_ic(void);
  void r3051_stage_rf(void);
  void r3051_stage_ex(void);
  void r3051_stage_dc(void);
  void r3051_stage_wb(void);

  // Pipeline Stage Helpers

  void r3051_stage_ex_00(R3051::Stage*);  
  void r3051_stage_ex_01(R3051::Stage*);  
  void r3051_stage_ex_cp(R3051::Stage*);

  //
  // Memory Interfaces

  uint32_t fetchByte(uint32_t);
  uint32_t fetchHalf(uint32_t);
  uint32_t fetchWord(uint32_t);

  void storeByte(uint32_t, uint32_t);
  void storeHalf(uint32_t, uint32_t);
  void storeWord(uint32_t, uint32_t);

  //
  // D-Cache Functions

  struct DCache {
    enum {
      DCACHE_SIZE = 0x100,
      DCACHE_LINE = 1,
      DCACHE_ELEM = DCACHE_SIZE / DCACHE_LINE
    };

    uint32_t lines[DCACHE_ELEM][DCACHE_LINE];
  };

  uint32_t dcacheFetch(uint32_t);
  void dcacheStore(uint32_t, uint32_t);

  //
  // I-Cache Functions

  struct ICache {
    enum {
      ICACHE_SIZE = 0x400,
      ICACHE_LINE = 4,
      ICACHE_ELEM = ICACHE_SIZE / ICACHE_LINE
    };

    uint32_t lines[ICACHE_ELEM][ICACHE_LINE];
    uint32_t tag[ICACHE_ELEM];
    uint32_t valid[ICACHE_ELEM];
  };

  uint32_t icacheFetch(uint32_t);
  void icacheStore(uint32_t, uint32_t);

  //
  // I-Cache

  uint32_t fetchInst(uint32_t);
};
