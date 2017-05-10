#ifndef __PSXACT_STATE_HPP__
#define __PSXACT_STATE_HPP__

#include <stdint.h>
#include "fifo.hpp"

struct cdrom_state_t {
  uint32_t interrupt_enable;
  uint32_t interrupt_request;
  uint32_t index;
  uint32_t command;
  bool has_command;

  fifo_t<uint8_t, 16> args_fifo;
  fifo_t<uint8_t, 16> resp_fifo;
  fifo_t<uint8_t, 16> data_fifo; // actually bigger, not sure the exact size
};

struct cpu_state_t {
  struct {
    uint32_t regs[16];
  } cop0;

  struct {
    uint32_t gp[32];
    uint32_t lo;
    uint32_t hi;
    uint32_t pc;
    uint32_t this_pc;
    uint32_t next_pc;
  } regs;

  uint32_t code;

  bool is_branch;
  bool is_branch_delay_slot;

  bool is_load;
  bool is_load_delay_slot;
  uint32_t load_index;
  uint32_t load_value;

  uint32_t i_stat;
  uint32_t i_mask;
};

struct dma_state_t {
  uint32_t dpcr = 0x07654321;
  uint32_t dicr = 0x00000000;

  struct {
    uint32_t address;
    uint32_t counter;
    uint32_t control;
  } channels[7];
};

struct gpu_state_t {
  uint32_t status = 0x14802000;
  uint32_t texture_window_mask_x;
  uint32_t texture_window_mask_y;
  uint32_t texture_window_offset_x;
  uint32_t texture_window_offset_y;
  uint32_t drawing_area_x1;
  uint32_t drawing_area_y1;
  uint32_t drawing_area_x2;
  uint32_t drawing_area_y2;
  uint32_t x_offset;
  uint32_t y_offset;
  uint32_t display_area_x;
  uint32_t display_area_y;
  uint32_t display_area_x1;
  uint32_t display_area_y1;
  uint32_t display_area_x2;
  uint32_t display_area_y2;
  bool textured_rectangle_x_flip;
  bool textured_rectangle_y_flip;

  struct {
    uint32_t buffer[16];
    int wr;
    int rd;
  } fifo;

  struct {
    struct {
      int x;
      int y;
      int w;
      int h;
    } reg;

    struct {
      bool active;
      int x;
      int y;
    } run;
  } cpu_to_gpu_transfer;

  struct {
    struct {
      int x;
      int y;
      int w;
      int h;
    } reg;

    struct {
      bool active;
      int x;
      int y;
    } run;
  } gpu_to_cpu_transfer;
};

struct input_state_t {
  uint32_t status;
};

struct spu_state_t {
};

struct timer_state_t {
  struct {
    uint16_t counter;
    uint16_t control;
    uint16_t compare;
    int divider;
  } timers[3];
};

struct system_state_t {
  cdrom_state_t cdrom_state;
  cpu_state_t cpu_state;
  dma_state_t dma_state;
  gpu_state_t gpu_state;
  input_state_t input_state;
  spu_state_t spu_state;
  timer_state_t timer_state;
};

#endif
