#ifndef __PSXACT_STATE_HPP__
#define __PSXACT_STATE_HPP__

#include <stdint.h>
#include <string>
#include "fifo.hpp"
#include "utility.hpp"

union color_t {
  struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t c;
  };

  uint32_t value;
};

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
  bool seek_unprocessed;

  fifo_t<uint8_t, 4> parameter;
  fifo_t<uint8_t, 4> response;
  fifo_t<uint8_t, 12> data;
  uint8_t data_buffer[4096];

  uint8_t command;
  bool command_is_new;
  bool busy;

  std::string game_file_name;
  FILE *game_file;

  typedef void (*stage_t)(cdrom_state_t *state);

  struct {
    stage_t stage;
    int timer;

    int interrupt_request;

    fifo_t<uint8_t, 4> parameter;
    fifo_t<uint8_t, 4> response;
    uint8_t command;
  } logic;

  struct {
    stage_t stage;
    int timer;
  } drive;

  struct {
    bool double_speed;
    bool read_whole_sector;
  } mode;
};

struct cop0_state_t {
  uint32_t regs[16];
};

struct cop2_state_t {
  struct {
    int32_t matrix[4][3][3];
    int32_t vector[4][3];
    int32_t ofx;
    int32_t ofy;
    int32_t h;
    int32_t dqa;
    int32_t dqb;
    int32_t zsf3;
    int32_t zsf4;
    uint32_t flag;
  } ccr;

  struct {
    int32_t vector[4][3];
    color_t rgbc;
    int32_t otz;
    int32_t ir0; //[4];
    int32_t sx[3];
    int32_t sy[3];
    int32_t sz[4];
    color_t rgb[3];
    int32_t res;
    int32_t mac[4];
    int32_t lzcs;
    int32_t lzcr;
  } gpr;
};

struct cpu_state_t {
  cop0_state_t cop0;
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
  uint32_t data_latch = 0;
  uint32_t status = 0x14802000;
  uint32_t texture_window_mask_x;
  uint32_t texture_window_mask_y;
  uint32_t texture_window_offset_x;
  uint32_t texture_window_offset_y;
  int32_t drawing_area_x1;
  int32_t drawing_area_y1;
  int32_t drawing_area_x2;
  int32_t drawing_area_y2;
  int32_t x_offset;
  int32_t y_offset;
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

struct mdec_state_t {
};

struct spu_state_t {
  uint16_t control;
  uint16_t status;

  uint16_t registers[24][8];

  utility::memory_t<19> sound_ram;

  uint32_t sound_ram_address;
  uint16_t sound_ram_address_latch;
  uint16_t sound_ram_transfer_control;
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
  mdec_state_t mdec_state;
  spu_state_t spu_state;
  timer_state_t timer_state;
};

#endif
