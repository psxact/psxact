#ifndef __PSXACT_STATE_HPP__
#define __PSXACT_STATE_HPP__

#include <stdint.h>
#include <string>
#include "fifo.hpp"

struct cdrom_sector_timecode_t {
  uint8_t minute;
  uint8_t second;
  uint8_t sector;
};

struct cdrom_state_t {
  int index;
  int interrupt_enable;
  int interrupt_request;

  cdrom_sector_timecode_t seek_timecode;
  cdrom_sector_timecode_t read_timecode;

  fifo_t<uint8_t, 4> parameter;
  fifo_t<uint8_t, 4> response;
  fifo_t<uint8_t, 12> data;

  uint8_t command;
  bool command_is_new;
  bool busy;

  std::string game_file_name;

  typedef void (*stage_t)(cdrom_state_t *state);

  struct {
    stage_t stage;
    int timer;

    int interrupt_request;

    fifo_t<uint8_t, 4> parameter;
    fifo_t<uint8_t, 4> response;
    uint8_t command;
  } control;

  struct {
    stage_t stage;
    int timer;
  } drive;

  struct {
    bool double_speed;
    bool read_whole_sector;
  } mode;
};


struct cop2_state_t {
  int32_t ofx;
  int32_t ofy;
  uint16_t h;
  int16_t dqa;
  int32_t dqb;
  int16_t zsf3;
  int16_t zsf4;
  int16_t matrices[3][3][3];
  int32_t control_vectors[4][3];
  uint32_t flags;

  struct color_t {
    uint8_t c;
    uint8_t b;
    uint8_t g;
    uint8_t r;
  };

  struct point_t {
    int16_t x;
    int16_t y;
  };

  int16_t v[4][3];
  int32_t mac[4];
  uint16_t otz;
  color_t rgb;
  int16_t ir[4];
  point_t xy_fifo[4];
  uint16_t z_fifo[4];
  color_t rgb_fifo[3];
  uint32_t lzcs;
  uint8_t lzcr;
  uint32_t reg_23;
};

struct cpu_state_t {
  struct {
    uint32_t regs[16];
  } cop0;

  cop2_state_t cop2;

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
