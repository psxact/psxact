#ifndef CONSOLE_HPP_
#define CONSOLE_HPP_

#include <cstdint>
#include "cdrom/core.hpp"
#include "cpu/core.hpp"
#include "dma/core.hpp"
#include "exp/expansion1.hpp"
#include "exp/expansion2.hpp"
#include "exp/expansion3.hpp"
#include "gpu/core.hpp"
#include "input/core.hpp"
#include "input/host-device.hpp"
#include "mdec/core.hpp"
#include "spu/core.hpp"
#include "timer/core.hpp"
#include "addressable.hpp"
#include "bios.hpp"
#include "interruptible.hpp"
#include "memory.hpp"
#include "memory-control.hpp"

namespace psx {

  struct input_params_t {
    psx::input::host_device_t device1;
    psx::input::host_device_t device2;
  };

  struct output_params_audio_t {
    int16_t *buffer;
    int32_t buffer_len;
  };

  struct output_params_video_t {
    uint32_t *buffer;
    int32_t width;
    int32_t height;
  };

  struct output_params_t {
    output_params_audio_t audio;
    output_params_video_t video;
  };

  class console_t final
    : public addressable_t {
    bios_t *bios;
    memory_t< mib(2) > *wram;

    cdrom::core_t *cdrom;
    timer::core_t *timer;
    cpu::core_t *cpu;
    dma::core_t *dma;
    exp::expansion1_t *exp1;
    exp::expansion2_t *exp2;
    exp::expansion3_t *exp3;
    gpu::core_t *gpu;
    input::core_t *input;
    mdec::core_t *mdec;
    spu::core_t *spu;
    memory_control_t *mem;
    int cycles;

    bool is_exe;

  public:
    console_t();
    ~console_t();

    uint32_t io_read(address_width_t width, uint32_t address);
    void io_write(address_width_t width, uint32_t address, uint32_t data);

    void run_for_one_frame(input_params_t &input, output_params_t &output);

  private:
    addressable_t &decode(uint32_t address);

    void get_audio_params(output_params_audio_t &params);
    void get_video_params(output_params_video_t &params);

    void load_exe(const char *game_file_name);
  };
}  // namespace psx

#endif  // CONSOLE_HPP_
