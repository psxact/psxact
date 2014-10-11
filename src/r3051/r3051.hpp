#pragma once

#include "stdafx.hpp"

class Bus;
class Cop0;

class R3051 {
public:
  struct Segment {
    uint32_t start;
    uint32_t length;
    uint32_t offset;
    bool cached;

    Segment(uint32_t, uint32_t, uint32_t, bool);
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

  Bus* bus;
  Cop0* cop0;

  Stage ic;
  Stage rf;
  Stage ex;
  Stage dc;
  Stage wb;

  //
  // Processor State

  R3051(void);
  ~R3051(void);

  void AttachBus(Bus*);
  void Step(void);

  //
  // Pipeline Stages

  void StageIc(void);
  void StageRf(void);
  void StageEx(void);
  void StageDc(void);
  void StageWb(void);

  // Pipeline Stage Helpers

  void StageEx00(R3051::Stage*);  
  void StageEx01(R3051::Stage*);  
  void StageExCp(R3051::Stage*);

  //
  // Memory Interfaces

  uint32_t FetchByte(uint32_t);
  uint32_t FetchHalf(uint32_t);
  uint32_t FetchWord(uint32_t);

  void StoreByte(uint32_t, uint32_t);
  void StoreHalf(uint32_t, uint32_t);
  void StoreWord(uint32_t, uint32_t);

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

  uint32_t DCacheFetch(uint32_t);
  void DCacheStore(uint32_t, uint32_t);

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

  uint32_t ICacheFetch(uint32_t);
  void ICacheStore(uint32_t, uint32_t);

  //
  // I-Cache

  uint32_t FetchInst(uint32_t);
};
