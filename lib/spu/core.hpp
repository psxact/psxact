#ifndef SPU_CORE_HPP_
#define SPU_CORE_HPP_

#include "cdrom/xa-adpcm-decoder.hpp"
#include "dma/comms.hpp"
#include "spu/voice.hpp"
#include "addressable.hpp"
#include "sound-ram.hpp"

namespace psx::spu {

  enum class spu_register {
    kon_lo          = 0x1f801d88,
    kon_hi          = 0x1f801d8a,
    koff_lo         = 0x1f801d8c,
    koff_hi         = 0x1f801d8e,
    pmon_lo         = 0x1f801d90,
    pmon_hi         = 0x1f801d92,
    endx_lo         = 0x1f801d9c,
    endx_hi         = 0x1f801d9a,
    ram_addr_irq    = 0x1f801da4,
    ram_addr        = 0x1f801da6,
    ram_data        = 0x1f801da8,
    control         = 0x1f801daa,
    status          = 0x1f801dae,
    cd_volume_left  = 0x1f801db0,
    cd_volume_right = 0x1f801db2
  };

  class core final
      : public addressable
      , public dma::comms {
    uint16_t registers[512];

    cdrom::xa_adpcm_decoder &xa_adpcm;
    int16_t cd_volume_left;
    int16_t cd_volume_right;

    sound_ram ram;
    uint32_t ram_address;
    uint32_t ram_address_irq;
    uint16_t ram_transfer_control;

    uint32_t capture_address;

    voice voices[24];
    uint32_t key_on;
    uint32_t key_off;
    uint32_t pmon;
    uint32_t endx;

    int prescaler;

    int16_t sample_buffer[2048];
    int sample_buffer_index;

  public:
    core(opts &o, cdrom::xa_adpcm_decoder &xa_adpcm);
    ~core();

    void tick(int amount);
    void step();

    int16_t *get_sample_buffer() const;
    uint32_t get_sample_buffer_index() const;
    void reset_sample();

    uint16_t get_register(spu_register reg);
    void put_register(spu_register reg, uint16_t value);
    void put_status_register();

    int dma_speed() override;
    bool dma_read_ready() override;
    bool dma_write_ready() override;
    uint32_t dma_read() override;
    void dma_write(uint32_t val) override;

    uint32_t io_read(address_width width, uint32_t address);
    void io_write(address_width width, uint32_t address, uint32_t data);

    // voice functions

    void voice_tick(int v, int32_t *l, int32_t *r);
    void voice_decoder_tick(int v);
  };
}

#endif
