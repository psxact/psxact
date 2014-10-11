#pragma once

class Spu {
  struct Voice {
  };

  Voice voice[24];

public:
  Spu(void);
  ~Spu(void);

  uint32_t Fetch(uint32_t);
  void Store(uint32_t, uint32_t);
};
