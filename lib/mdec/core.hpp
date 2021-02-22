#ifndef MDEC_CORE_HPP_
#define MDEC_CORE_HPP_

#include "dma/comms.hpp"
#include "util/fifo.hpp"
#include "addressable.hpp"

namespace psx::mdec {

  class core final
      : public addressable
      , public dma::comms {
  private:
    uint32_t status;
    bool dma_0_enabled;
    bool dma_1_enabled;

    struct {
      uint32_t val;
      uint32_t out;
      uint16_t len;
      bool run;
    } cmd;

    util::fifo< uint32_t, 5 > data_in;
    util::fifo< uint32_t, 5 > data_out;

    uint8_t color_table[64];
    uint8_t light_table[64];
    int16_t scale_table[64];

  public:
    core(opts &o);

    uint32_t get_status() const;

    void put_command(uint32_t val);
    void put_control(uint32_t val);
    void put_parameter(uint32_t val);

    void run_command();

    void fill_color_table();
    void fill_light_table();
    void fill_scale_table();

    uint32_t io_read(address_width width, uint32_t address) override;
    void io_write(address_width width, uint32_t address, uint32_t data) override;

    int dma_speed() override;
    bool dma_read_ready() override;
    bool dma_write_ready() override;
    uint32_t dma_read() override;
    void dma_write(uint32_t val) override;
  };
}

#endif
