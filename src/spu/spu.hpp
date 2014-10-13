#pragma once

class Bus;

class Spu {
  struct Voice {
  };

  Bus* bus;
  Voice voice[24];

public:
  Spu(void);
  ~Spu(void);

  void AttachBus(Bus*);

  uint32_t Fetch(uint32_t);
  void Store(uint32_t, uint32_t);
};
