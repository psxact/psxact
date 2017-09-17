#ifndef __PSXACT_TIMER_CORE_HPP__
#define __PSXACT_TIMER_CORE_HPP__

#include "../system_core.hpp"

namespace psxact {
namespace timer {

  struct unit {
    uint16_t counter;
    bool running;

    // compare

    struct {
      bool reached;
      bool irq_enable;
      bool reset_counter;
      uint16_t value;
    } compare;

    // maximum

    struct {
      bool reached;
      bool irq_enable;
    } maximum;

    // irq

    struct {
      bool repeat;
      bool enable;
      bool toggle;
      bool bit;
    } irq;

    int synch_enable;
    int synch_mode;

    struct {
      bool enable;
      int cycles;
      int single;
      int period;
    } prescaler;
  };

  struct core {
    bool in_hblank;
    bool in_vblank;

    unit units[3];

    core();

    uint32_t io_read(bus_width_t width, uint32_t address);

    void io_write(bus_width_t width, uint32_t address, uint32_t data);

    void tick();

    void hblank(bool active);

    void vblank(bool active);

  private:
    void unit_init(int n, int single, int period);

    void unit_irq(int n);

    void unit_tick(int n);

    void unit_prescale(int n);

    uint16_t unit_get_compare(int n);

    uint16_t unit_get_control(int n);

    uint16_t unit_get_counter(int n);

    void unit_set_compare(int n, uint16_t data);

    void unit_set_control(int n, uint16_t data);

    void unit_set_counter(int n, uint16_t data);
  };

}
}

#endif // __PSXACT_TIMER_CORE_HPP__
