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
#include "memory/bios.hpp"
#include "memory/memory-control.hpp"
#include "memory/wram.hpp"
#include "spu/core.hpp"
#include "timer/core.hpp"
#include "addressable.hpp"
#include "interruptible.hpp"

namespace psx {

  struct input_params {
    psx::input::host_device device1;
    psx::input::host_device device2;
  };

  struct output_params_audio {
    int16_t *buffer;
    int32_t buffer_len;
  };

  struct output_params_video {
    uint32_t *buffer;
    int32_t width;
    int32_t height;
  };

  struct output_params {
    output_params_audio audio;
    output_params_video video;
  };

  class console final
    : public addressable {
    memory::bios *bios;
    memory::wram *wram;
    memory::memory_control *mem;

    cdrom::core *cdrom;
    cdrom::xa_adpcm_decoder *xa_adpcm;
    timer::core *timer;
    cpu::core *cpu;
    dma::core *dma;
    exp::expansion1 *exp1;
    exp::expansion2 *exp2;
    exp::expansion3 *exp3;
    gpu::core *gpu;
    input::core *input;
    mdec::core *mdec;
    spu::core *spu;
    int cycles;

    bool is_exe;

  public:
    console();
    ~console();

    uint32_t io_read(address_width width, uint32_t address);
    void io_write(address_width width, uint32_t address, uint32_t data);

    void run_for_one_frame(input_params &input, output_params &output);

  private:
    addressable &decode(uint32_t address);

    void get_audio_params(output_params_audio &params);
    void get_video_params(output_params_video &params);

    void load_exe(const char *game_file_name);
  };
}

#endif
