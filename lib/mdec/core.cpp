#include "mdec/core.hpp"

#include <cstdio>

using namespace psx::mdec;

core_t::core_t(bool log_enabled)
  : addressable_t("mdec", log_enabled) {
}

void core_t::send_command(uint32_t data) {
  command = command_t((data >> 29) & 7);

  output_depth  = (data >> 27) & 3;
  output_signed = (data >> 26) & 1;
  output_bit15  = (data >> 25) & 1;

  switch (command) {
    case command_t::decode_mb:
      log("command: decode_mb");
      parameter.index = 0;
      parameter.total = (data & 0xffff);
      break;

    case command_t::set_iqtab:
      log("command: set_iqtab");
      parameter.index = 0;
      parameter.total = (data & 1) ? 32 : 16;
      break;

    case command_t::set_scale:
      log("command: set_scale");
      parameter.index = 0;
      parameter.total = 32;
      break;

    default:
      log("command: %08x (unhandled)", unsigned(command));
      break;
  }
}

void core_t::send_parameter(int n, uint32_t data) {
  switch (command) {
    case command_t::decode_mb:
      return;

    case command_t::set_iqtab:
      return n < 16
        ? send_light_tab(n, data)
        : send_color_tab(n - 16, data);

    case command_t::set_scale:
      return send_scale_tab(n, data);
  }
}

void core_t::send_color_tab(int n, uint32_t data) {
  log("color table(%d): 0x%08x", n, data);

  color_tab[(n * 4) + 0] = uint8_t(data >> (8 * 0));
  color_tab[(n * 4) + 1] = uint8_t(data >> (8 * 1));
  color_tab[(n * 4) + 2] = uint8_t(data >> (8 * 2));
  color_tab[(n * 4) + 3] = uint8_t(data >> (8 * 3));
}

void core_t::send_light_tab(int n, uint32_t data) {
  log("light table(%d): 0x%08x", n, data);

  light_tab[(n * 4) + 0] = uint8_t(data >> (8 * 0));
  light_tab[(n * 4) + 1] = uint8_t(data >> (8 * 1));
  light_tab[(n * 4) + 2] = uint8_t(data >> (8 * 2));
  light_tab[(n * 4) + 3] = uint8_t(data >> (8 * 3));
}

void core_t::send_scale_tab(int n, uint32_t data) {
  log("scale table(%d): 0x%08x", n, data);

  scale_tab[(n * 2) + 0] = int16_t(data >> (8 * 0));
  scale_tab[(n * 2) + 1] = int16_t(data >> (8 * 2));
}

uint32_t core_t::io_read_word(uint32_t address) {
  log("io_read_word(0x%08x)", address);

  switch (address) {
    case MDEC_DATA: {
      uint32_t response = 0;
      log("data=%08x", response);

      return response;
    }

    case MDEC_STAT: {
      uint32_t response =
        (data_out.is_empty() << 31) |
        (data_in.is_full() << 30) |
        ((parameter.index != parameter.total) << 29) |
        (enable_data_in << 28) |
        (enable_data_out << 27) |
        (output_depth << 25) |
        (output_signed << 24) |
        (output_bit15 << 23) |
        (int(block) << 16) |
        (parameter.total - parameter.index);

      log("stat=%08x", response);
      return response;
    }
  }

  return addressable_t::io_read_word(address);
}

void core_t::io_write_word(uint32_t address, uint32_t data) {
  switch (address) {
    case MDEC_COMMAND:
      if (parameter.index == parameter.total) {
        log("io_cmd(0x%08x)", data);
        send_command(data);
      }
      else {
        send_parameter(parameter.index, data);
        parameter.index++;
      }
      break;

    case MDEC_CONTROL:
      log("io_ctl(0x%08x)", data);

      if (data & (1 << 31)) {
        data_out.clear();
        data_in.clear();
        parameter.index = 0;
        parameter.total = 0;
        enable_data_in  = 0;
        enable_data_out = 0;
        output_depth = 0;
        output_signed = 0;
        output_bit15 = 0;
        block = block_t::cr;
      }
      else {
        enable_data_in  = (data >> 30) & 1;
        enable_data_out = (data >> 29) & 1;
      }
      break;

    default:
      log("io_write_word(0x%08x, 0x%08x)", address, data);
      break;
  }
}

int core_t::dma_speed() {
  return 1;
}

bool core_t::dma_ready() {
  return true;
}

uint32_t core_t::dma_read() {
  assert(0 && "MDEC DMA read isn't implemented.");
}

void core_t::dma_write(uint32_t val) {
  io_write_word(MDEC_COMMAND, val);
}
