#include "mdec/core.hpp"

#include <cstdio>
#include "utility.hpp"

using namespace psx::mdec;

core_t::core_t(bool log_enabled)
  : memory_component_t("mdec", log_enabled) {
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
      uint32_t response = status
        | ((parameter.index != parameter.total) << 29);

      log("stat=%08x", response);
      return response;
    }
  }

  return core_t::io_read_word(address);
}

void core_t::io_write_word(uint32_t address, uint32_t data) {
  log("io_write_word(0x%08x, 0x%08x)", address, data);

  switch (address) {
    case MDEC_COMMAND:
      if (parameter.index == parameter.total) {
        send_command(data);
      }
      else {
        send_parameter(parameter.index, data);
        parameter.index++;
      }
      break;

    case MDEC_CONTROL:
      if (data & (1 << 31)) {
        log("reset");

        status = 0x80040000;
        parameter.index = 0;
        parameter.total = 0;
        enable_data_in  = 0;
        enable_data_out = 0;
      }
      else {
        log("enabling DMA: %08x", data);

        enable_data_in = (data >> 28) & 1;
        enable_data_out = (data >> 27) & 1;
      }
      break;
  }
}


void core_t::send_command(uint32_t data) {
  command = command_t((data >> 29) & 7);

  status = (status & ~0x7800000) | ((data >> 2) & 0x7800000);

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
  color_tab[(n * 4) + 0] = uint8_t(data >> (8 * 0));
  color_tab[(n * 4) + 1] = uint8_t(data >> (8 * 1));
  color_tab[(n * 4) + 2] = uint8_t(data >> (8 * 2));
  color_tab[(n * 4) + 3] = uint8_t(data >> (8 * 3));
}


void core_t::send_light_tab(int n, uint32_t data) {
  light_tab[(n * 4) + 0] = uint8_t(data >> (8 * 0));
  light_tab[(n * 4) + 1] = uint8_t(data >> (8 * 1));
  light_tab[(n * 4) + 2] = uint8_t(data >> (8 * 2));
  light_tab[(n * 4) + 3] = uint8_t(data >> (8 * 3));
}


void core_t::send_scale_tab(int n, uint32_t data) {
  scale_tab[(n * 2) + 0] = int16_t(data >> (8 * 0));
  scale_tab[(n * 2) + 1] = int16_t(data >> (8 * 2));
}
