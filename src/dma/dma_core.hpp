#ifndef __PSXACT_DMA_CORE_HPP__
#define __PSXACT_DMA_CORE_HPP__

#include <cstdint>
#include "../state.hpp"

namespace dma {
  uint32_t io_read(dma_state_t *state, int width, uint32_t address);

  void io_write(dma_state_t *state, int width, uint32_t address, uint32_t data);

  void main(dma_state_t *state);

  void irq_channel(dma_state_t *state, int n);

  void run_channel(dma_state_t *state, int n);
}

#endif // __PSXACT_DMA_CORE_HPP__
