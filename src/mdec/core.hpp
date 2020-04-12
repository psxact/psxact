#ifndef MDEC_CORE_HPP_
#define MDEC_CORE_HPP_

#include "util/fifo.hpp"
#include "memory-component.hpp"

namespace psx::mdec {

enum {
  MDEC_DATA = 0x1f801820,
  MDEC_STAT = 0x1f801824,
  MDEC_COMMAND = 0x1f801820,
  MDEC_CONTROL = 0x1f801824
};

enum class block_t {
  y1 = 0,
  y2 = 1,
  y3 = 2,
  y4 = 3,
  cr = 4,
  cb = 5
};

enum class command_t {
  decode_mb = 1,
  set_iqtab = 2,
  set_scale = 3
};

class core_t final : public memory_component_t {
 private:
  block_t block;
  command_t command;

  util::fifo_t<uint8_t, 6> data_in;
  util::fifo_t<uint8_t, 6> data_out;

  uint32_t output_depth;
  bool output_signed;
  bool output_bit15;

  struct {
    uint32_t index;
    uint32_t total;
  } parameter;

  uint8_t light_tab[64];
  uint8_t color_tab[64];
  int16_t scale_tab[64];

  bool enable_data_in;
  bool enable_data_out;

 public:
  core_t(bool log_enabled);

  uint32_t io_read_word(uint32_t address);

  void io_write_word(uint32_t address, uint32_t data);

  void send_command(uint32_t data);

  void send_parameter(int n, uint32_t data);

  void send_color_tab(int n, uint32_t data);

  void send_light_tab(int n, uint32_t data);

  void send_scale_tab(int n, uint32_t data);
};

}  // namespace psx::mdec

#endif  // MDEC_CORE_HPP_
