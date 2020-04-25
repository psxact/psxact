#ifndef SPU_CORE_HPP_
#define SPU_CORE_HPP_

#include "spu/voice.hpp"
#include "addressable.hpp"
#include "sound-ram.hpp"
#include "memory.hpp"

namespace psx::spu {

  enum class register_t {
    kon_lo = 0x1F801D88,
    kon_hi = 0x1F801D8A,
    pmon_lo = 0x1F801D90,
    pmon_hi = 0x1F801D92,
    endx_lo = 0x1F801D9C,
    endx_hi = 0x1F801D9A,
    ram_addr_irq = 0x1F801DA4,
    ram_addr = 0x1F801DA6,
    ram_data = 0x1F801DA8
  };

  class core_t final : public addressable_t {
    uint16_t registers[512];

    sound_ram_t ram;
    uint32_t ram_address;
    uint32_t ram_address_irq;
    uint16_t ram_transfer_control;

    voice_t voices[24];
    uint32_t key_on;
    uint32_t pmon;
    uint32_t endx;

    int prescaler;

    int16_t sample_buffer[2048];
    int sample_buffer_index;

  public:
    core_t(bool log_enabled);
    ~core_t();

    void run(int amount);
    void tick();

    const int16_t *get_sample() const;

    uint16_t get_register(register_t reg);
    void put_register(register_t reg, uint16_t value);

    uint16_t io_read_half(uint32_t address);
    void io_write_half(uint32_t address, uint16_t data);
    void io_write_word(uint32_t address, uint32_t data);

    // voice functions

    void voice_tick(int v, int32_t *l, int32_t *r);
    void voice_decoder_tick(int v);
  };
}  // namespace psx::spu

#endif  // SPU_CORE_HPP_
