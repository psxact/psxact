#ifndef DMA_COMMS_HPP_
#define DMA_COMMS_HPP_

#include <cstdint>

namespace psx {

  /// Interface to implement for communication with a DMA channel.
  struct dma_comms_t {
    /// Returns the number of cycles needed for one access.
    virtual int dma_speed() = 0;

    /// Returns true when data is ready to be read.
    virtual bool dma_read_ready() = 0;

    /// Returns true when data is ready to be written.
    virtual bool dma_write_ready() = 0;

    /// Returns data when requested by the DMA channel.
    virtual uint32_t dma_read() = 0;

    /// Writes data from the DMA channel.
    virtual void dma_write(uint32_t val) = 0;
  };
}

#endif  // DMA_COMMS_HPP_
