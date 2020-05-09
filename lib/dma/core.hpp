#ifndef DMA_CORE_HPP_
#define DMA_CORE_HPP_

#include "util/wire.hpp"
#include "addressable.hpp"
#include "dma-comms.hpp"

namespace psx::dma {

struct channel_t {
  uint32_t address;
  uint32_t counter;
  uint32_t control;
  dma_comms_t *comms;
};

class core_t final : public addressable_t {
  util::wire_t irq;
  addressable_t &memory;

  /// The number of channels in `priority_lut'
  int priority_len = {};

  /// The prioritized list of channels
  int priority_lut[7] = {};

  /// Priority control register
  uint32_t pcr = {};

  /// Interrupt control register
  uint32_t icr = {};

  /// The nominal list of channels
  channel_t channels[7] = {};

 public:
  core_t(util::wire_t irq, addressable_t &memory);

  void attach(int n, dma_comms_t *comms);

  int tick();
  int tick_channel(int n);
  int tick_sync_mode_otc();
  int tick_sync_mode_0(int n);
  int tick_sync_mode_1(int n);
  int tick_sync_mode_2(int n);

  uint8_t channel_priority(int n) const;

  void put_icr(uint32_t val);
  void put_pcr(uint32_t val);

  uint32_t io_read(address_width_t width, uint32_t address);
  void io_write(address_width_t width, uint32_t address, uint32_t data);

  uint32_t get32(uint32_t address);

  void irq_channel(int32_t n);

  void update_irq_active_flag();
};

}  // namespace psx::dma

#endif  // DMA_CORE_HPP_
