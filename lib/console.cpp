#include "console.hpp"

#include <cassert>
#include <cstring>
#include <exception>
#include "util/blob.hpp"
#include "util/range.hpp"
#include "util/wire.hpp"
#include "args.hpp"

using namespace psx;
using namespace psx::util;

console_t::console_t()
  : addressable_t("console", false) {
  bios = new bios_t();
  wram = new memory_t< mib(2) >("wram");

  wire_t irq1;
  irq1.recv_rise([&]() { cpu->interrupt(interrupt_type_t::gpu); });

  wire_t irq3;
  irq3.recv_rise([&]() { cpu->interrupt(interrupt_type_t::dma); });

  wire_t irq4;
  irq4.recv_rise([&]() { cpu->interrupt(interrupt_type_t::timer0); });

  wire_t irq5;
  irq5.recv_rise([&]() { cpu->interrupt(interrupt_type_t::timer1); });

  wire_t irq6;
  irq6.recv_rise([&]() { cpu->interrupt(interrupt_type_t::timer2); });

  wire_t gpu_hblank;
  gpu_hblank.recv_rise([&]() { timer->enter_hblank(); });
  gpu_hblank.recv_fall([&]() { timer->leave_hblank(); });

  wire_t gpu_vblank;
  gpu_vblank.recv_rise([&]() { cpu->interrupt(interrupt_type_t::vblank); });
  gpu_vblank.recv_rise([&]() { timer->enter_vblank(); });
  gpu_vblank.recv_fall([&]() { timer->leave_vblank(); });

  timer = new timer::core_t(irq4, irq5, irq6);
  cpu = new cpu::core_t(*this);
  dma = new dma::core_t(irq3, *this);
  exp1 = new exp::expansion1_t();
  exp2 = new exp::expansion2_t();
  exp3 = new exp::expansion3_t();
  gpu = new gpu::core_t(irq1, gpu_hblank, gpu_vblank);
  cdrom = new cdrom::core_t(*cpu, args::game_file_name);
  input = new input::core_t(*cpu);
  mdec = new mdec::core_t();
  spu = new spu::core_t();
  mem = new memory_control_t();

  dma->attach(0, mdec);
  dma->attach(1, mdec);
  dma->attach(2, gpu);
  dma->attach(3, cdrom);
  dma->attach(4, spu);
  dma->attach(5, nullptr); // PIO
  dma->attach(6, nullptr); // OTC - special cased in the DMA code.

  is_exe = !!(strstr(args::game_file_name, ".exe") || strstr(args::game_file_name, ".psexe"));
}

console_t::~console_t() {
  delete bios;
  delete wram;
  delete cdrom;
  delete timer;
  delete cpu;
  delete dma;
  delete exp1;
  delete exp2;
  delete exp3;
  delete gpu;
  delete input;
  delete mdec;
  delete spu;
  delete mem;
}

addressable_t &console_t::decode(uint32_t address) {
#define between(min, max) \
  range::between<(min), (max)>(address)

  if (between(0x1f801800, 0x1f801803)) {
    if (is_exe) {
      is_exe = false;
      load_exe(args::game_file_name);
    }

    return *cdrom;
  }

  if (between(0x00000000, 0x007fffff)) { return *wram; }
  if (between(0x1fc00000, 0x1fc7ffff)) { return *bios; }
  if (between(0x1f801040, 0x1f80104f)) { return *input; }
  if (between(0x1f801070, 0x1f801077)) { return *cpu; }
  if (between(0x1f801080, 0x1f8010ff)) { return *dma; }
  if (between(0x1f801100, 0x1f80113f)) { return *timer; }
  if (between(0x1f801800, 0x1f801803)) { return *cdrom; }
  if (between(0x1f801810, 0x1f801817)) { return *gpu; }
  if (between(0x1f801820, 0x1f801827)) { return *mdec; }
  if (between(0x1f801c00, 0x1f801fff)) { return *spu; }
  if (between(0x1f000000, 0x1f7fffff)) { return *exp1; }
  if (between(0x1f802000, 0x1f802fff)) { return *exp2; }
  if (between(0x1fa00000, 0x1fbfffff)) { return *exp3; }

#undef between

  return *mem;
}

uint32_t console_t::io_read(address_width_t width, uint32_t address) {
  return decode(address).io_read(width, address);
}

void console_t::io_write(address_width_t width, uint32_t address, uint32_t data) {
  return decode(address).io_write(width, address, data);
}

void console_t::run_for_one_frame(input_params_t &i, output_params_t &o) {
  input->latch(i.device1, i.device2);

  while (1) {
    int amount = cpu->tick() + dma->tick();

    spu->run(amount);
    timer->run(amount);
    cdrom->tick(amount);
    input->tick(amount);

    if (gpu->run(amount)) {
      break;
    }
  }

  get_audio_params(o.audio);
  get_video_params(o.video);
}

void console_t::get_audio_params(output_params_audio_t &params) {
  params.buffer = spu->get_sample_buffer();
  params.buffer_len = spu->get_sample_buffer_index();

  spu->reset_sample();
}

void console_t::get_video_params(output_params_video_t &params) {
  params.buffer = gpu->get_video_buffer();
  params.width = int(gpu->get_h_resolution());
  params.height = 480;
}

void console_t::load_exe(const char *game_file_name) {
  // load the exe into ram
  if (blob_t *blob = blob_t::from_file(game_file_name)) {
    cpu->set_pc(blob->read_word(0x10));
    cpu->set_register(4, 1);
    cpu->set_register(5, 0);
    cpu->set_register(28, blob->read_word(0x14));
    cpu->set_register(29, blob->read_word(0x30) + blob->read_word(0x34));
    cpu->set_register(30, blob->read_word(0x30) + blob->read_word(0x34));

    int text_start = blob->read_word(0x18);
    int text_count = blob->read_word(0x1c);

    printf("Loading executable into WRAM..\n");
    printf("  PC: $%08x\n", cpu->get_pc());
    printf("  GP: $%08x\n", cpu->get_register(28));
    printf("  SP: $%08x\n", cpu->get_register(29));
    printf("  FP: $%08x\n", cpu->get_register(30));

    for (int i = 0; i < text_count; i++) {
      wram->io_write(address_width_t::byte, text_start + i, blob->read_byte(0x800 + i));
    }
  }
}
