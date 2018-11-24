// Copyright 2018 psxact

#ifndef DMA_CORE_HPP_
#define DMA_CORE_HPP_

#include "interrupt-access.hpp"
#include "memory-access.hpp"
#include "memory-component.hpp"

namespace psx {
namespace dma {

class core_t : public memory_component_t {
  interrupt_access_t *irq;
  memory_access_t *memory;

  uint32_t dpcr = 0x07654321;
  uint32_t dicr = 0x00000000;

  struct {
    uint32_t address;
    uint32_t counter;
    uint32_t control;
  } channels[7];

 public:
  core_t(interrupt_access_t *irq, memory_access_t *memory);

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

}  // namespace dma
}  // namespace psx

#endif  // DMA_CORE_HPP_
