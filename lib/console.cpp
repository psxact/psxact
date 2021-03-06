#include "console.hpp"

#include "util/blob.hpp"
#include "util/range.hpp"
#include "util/wire.hpp"

using namespace psx;
using namespace psx::util;

console::console(opts &o)
  : addressable(o, component::console)
  , opt(o) {
  bios = new memory::bios(o);
  wram = new memory::wram(o);

  wire irq1;
  irq1.recv_rise([&]() { cpu->interrupt(interrupt_type::gpu); });

  wire irq2;
  irq2.recv_rise([&]() { cpu->interrupt(interrupt_type::cdrom); });

  wire irq3;
  irq3.recv_rise([&]() { cpu->interrupt(interrupt_type::dma); });

  wire irq4;
  irq4.recv_rise([&]() { cpu->interrupt(interrupt_type::timer0); });

  wire irq5;
  irq5.recv_rise([&]() { cpu->interrupt(interrupt_type::timer1); });

  wire irq6;
  irq6.recv_rise([&]() { cpu->interrupt(interrupt_type::timer2); });

  wire gpu_hblank;
  gpu_hblank.recv_rise([&]() { timer->enter_hblank(); });
  gpu_hblank.recv_fall([&]() { timer->leave_hblank(); });

  wire gpu_vblank;
  gpu_vblank.recv_rise([&]() { cpu->interrupt(interrupt_type::vblank); });
  gpu_vblank.recv_rise([&]() { timer->enter_vblank(); });
  gpu_vblank.recv_fall([&]() { timer->leave_vblank(); });

  xa_adpcm = new cdrom::xa_adpcm_decoder();

  timer = new timer::core(o, irq4, irq5, irq6);
  cpu = new cpu::core(o, *this);
  dma = new dma::core(o, irq3, *this);
  exp1 = new exp::expansion1(o);
  exp2 = new exp::expansion2(o);
  exp3 = new exp::expansion3(o);
  gpu = new gpu::core(o, irq1, gpu_hblank, gpu_vblank);
  cdrom = new cdrom::core(o, irq2, *xa_adpcm);
  input = new input::core(o, *cpu);
  mdec = new mdec::core(o);
  spu = new spu::core(o, *xa_adpcm);
  mem = new memory::memory_control(o);

  dma->attach(0, mdec);
  dma->attach(1, mdec);
  dma->attach(2, gpu);
  dma->attach(3, cdrom);
  dma->attach(4, spu);
  dma->attach(5, nullptr); // PIO
  dma->attach(6, nullptr); // OTC - special cased in the DMA code.

  load_exe_pending = o.get_game_file_type() == game_type::psexe;
}

console::~console() {
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
  delete xa_adpcm;
}

addressable &console::decode(uint32_t address) {
#define between(min, max) \
  range::between<(min), (max)>(address)

  if (between(0x1f801800, 0x1f801803)) {
    if (load_exe_pending) {
      load_exe_pending = false;
      load_exe(opt.get_game_file().value());
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

uint32_t console::io_read(address_width width, uint32_t address) {
  return decode(address).io_read(width, address);
}

void console::io_write(address_width width, uint32_t address, uint32_t data) {
  return decode(address).io_write(width, address, data);
}

void console::run_for_one_frame(input_params &i, output_params &o) {
  input->latch(i.device1, i.device2);

  while (1) {
    int amount = cpu->tick() + dma->tick();

    spu->tick(amount);
    timer->tick(amount);
    cdrom->tick(amount);
    input->tick(amount);

    if (gpu->tick(amount)) {
      break;
    }
  }

  get_audio_params(o.audio);
  get_video_params(o.video);
}

void console::get_audio_params(output_params_audio &params) {
  params.buffer = spu->get_sample_buffer();
  params.buffer_len = spu->get_sample_buffer_index();

  spu->reset_sample();
}

void console::get_video_params(output_params_video &params) {
  params.buffer = gpu->get_video_buffer();
  params.width = int(gpu->get_h_resolution());
  params.height = 480;
}

void console::load_exe(FILE *game_file) {
  // load the exe into ram
  if (blob *blob = blob::from_file(game_file)) {
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
    printf("  Text Start: %08x\n", text_start);
    printf("  Text Count: %08x\n", text_count);

    for (int i = 0; i < text_count; i++) {
      wram->io_write(address_width::byte, text_start + i, blob->read_byte(0x800 + i));
    }

    delete blob;
  }
}
