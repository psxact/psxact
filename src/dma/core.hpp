#ifndef DMA_CORE_HPP_
#define DMA_CORE_HPP_

#include "addressable.hpp"
#include "interrupt-access.hpp"

namespace psx::dma {

struct channel_t {
  uint32_t address;
  uint32_t counter;
  uint32_t control;
};

class core_t final : public addressable_t {
  interrupt_access_t *irq;
  addressable_t *memory;

  uint32_t dpcr;
  uint32_t dicr;

  channel_t channels[7];

 public:
  core_t(interrupt_access_t *irq, addressable_t *memory, bool log_enabled);

  uint32_t io_read_word(uint32_t address);

  void io_write_word(uint32_t address, uint32_t data);

  void main();

  void irq_channel(int32_t n);

  void run_channel(int32_t n);

  void run_channel_0();

  void run_channel_1();

  void run_channel_2_data_read();

  void run_channel_2_data_write();

  void run_channel_2_list();

  void run_channel_3();

  void run_channel_4_write();

  void run_channel_6();

  void update_irq_active_flag();
};

}  // namespace psx::dma

#endif  // DMA_CORE_HPP_
