#include "stdafx.h"

#ifndef PSX_GPU_H
#define PSX_GPU_H

void gpu_write_gp0(uint32_t);
void gpu_write_gp1(uint32_t);

uint32_t gpu_read_resp(void);
uint32_t gpu_read_stat(void);

struct coordinate {
  uint32_t x;
  uint32_t y;
};

#endif
