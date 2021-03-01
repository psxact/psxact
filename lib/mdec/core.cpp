#include "mdec/core.hpp"

#include "util/int.hpp"
#include "util/panic.hpp"
#include "util/uint.hpp"
#include "timing.hpp"

using namespace psx::mdec;
using namespace psx::util;

core::core(opts &o)
  : addressable(o, component::mdec) {
}

uint32_t core::get_status() const {
  auto bit_31 = data_out.is_empty();
  auto bit_30 = data_in.is_full();
  auto bit_29 = cmd.run;
  auto bit_28 = dma_0_enabled && cmd.run && !data_in.is_full();
  auto bit_27 = dma_1_enabled && cmd.run && !data_out.is_empty(); // TODO: probably not just while a command is active?
  auto bit_23 = cmd.out;
  auto bit_00 = cmd.len;

  return status
      | (bit_31 << 31)
      | (bit_30 << 30)
      | (bit_29 << 29)
      | (bit_28 << 28)
      | (bit_27 << 27)
      | (bit_23 << 23)
      | (bit_00 <<  0);
}

void core::put_command(uint32_t val) {
  cmd.val = (val >> 29) & 0x7;
  cmd.out = (val >> 25) & 0xf;
  cmd.len = (val >>  0) & 0xffff;

  if (cmd.val == 2) {
    cmd.len = (val & 1) ? 32 : 16;
  }

  if (cmd.val == 3) {
    cmd.len = 32;
  }

  if (cmd.val == 1 || cmd.val == 2 || cmd.val == 3) {
    cmd.run = 1;
    cmd.len--;
  }
}

void core::put_parameter(uint32_t val) {
  if (data_in.is_full()) {
    // For at least command 1, the MDEC appears to wait for a full input FIFO
    // before processing anything.
    run_command();
  }

  // write parameter
  data_in.write(val);

  if ((cmd.len--) == 0) {
    // run a command here?
    run_command();
    cmd.run = false;
  }
}

void core::put_control(uint32_t val) {
  dma_0_enabled = (val >> 30) & 1;
  dma_1_enabled = (val >> 29) & 1;

  if (val & 0x80000000) {
    status = 0x40000;
    data_in.clear();
    data_out.clear();
    cmd = {};
    dma_0_enabled = 0;
    dma_1_enabled = 0;
  }
}

void core::run_command() {
  if (cmd.val == 1) {
    LOG_INFO("running command 1");

    PANIC_IF(data_in.size() != 32,
			"mdec command with wrong parameter count");

    data_in.clear();
  }

  if (cmd.val == 2) {
    LOG_INFO("running command 2");

    uint32_t len = data_in.size();
    if (len != 16 && len != 32) {
      PANIC("Unhandled MDEC FIFO size for command 2.");
    } else if (len == 16) {
      fill_light_table();
    } else if (len == 32) {
      fill_light_table();
      fill_color_table();
    }
  }

  if (cmd.val == 3) {
    LOG_INFO("running command 3");
    fill_scale_table();
  }

  PANIC_IF(data_in.is_empty() == false,
		"junk data left in mdec parameter fifo");
}

void core::fill_light_table() {
  // We can't just assert the actual data size here because color data might
  // follow.

  PANIC_IF(data_in.size() != 16 && data_in.size() != 32,
		"fill light table with wrong parameter count");

  for (int i = 0; i < 64; ) {
    uint32_t val = data_in.read();
    light_table[i++] = uint_t<8>::trunc(val >> (8 * 0));
    light_table[i++] = uint_t<8>::trunc(val >> (8 * 1));
    light_table[i++] = uint_t<8>::trunc(val >> (8 * 2));
    light_table[i++] = uint_t<8>::trunc(val >> (8 * 3));
  }
}

void core::fill_color_table() {
  PANIC_IF(data_in.size() != 16,
		"fill color table with wrong parameter count");

  for (int i = 0; i < 64; ) {
    uint32_t val = data_in.read();
    color_table[i++] = uint_t<8>::trunc(val >> (8 * 0));
    color_table[i++] = uint_t<8>::trunc(val >> (8 * 1));
    color_table[i++] = uint_t<8>::trunc(val >> (8 * 2));
    color_table[i++] = uint_t<8>::trunc(val >> (8 * 3));
  }
}

void core::fill_scale_table() {
  PANIC_IF(data_in.size() != 32,
		"fill scale table with wrong parameter count");

  for (int i = 0; i < 64; ) {
    uint32_t val = data_in.read();
    scale_table[i++] = int_t<16>::trunc(val);
    scale_table[i++] = int_t<16>::trunc(val >> 16);
  }
}

uint32_t core::io_read(address_width width, uint32_t address) {
  timing::add_cpu_time(4);

  if (width == address_width::word) {
    switch (address) {
      case 0x1f801824: {
        return get_status();
      }
    }
  }

  return addressable::io_read(width, address);
}

void core::io_write(address_width width, uint32_t address, uint32_t data) {
  timing::add_cpu_time(4);

  if (width == address_width::word) {
    switch (address) {
      case 0x1f801820: {
        if (cmd.run) {
          return put_parameter(data);
        } else {
          return put_command(data);
        }
      }

      case 0x1f801824: {
        return put_control(data);
      }
    }
  }

  return addressable::io_write(width, address, data);
}

int core::dma_speed() {
  return 1;
}

bool core::dma_read_ready() {
  return true;
}

bool core::dma_write_ready() {
  return true;
}

uint32_t core::dma_read() {
  // panic("MDEC DMA read isn't implemented.");
  return 0;
}

void core::dma_write(uint32_t val) {
  io_write(address_width::word, 0x1f801820, val);
}
