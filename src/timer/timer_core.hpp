#ifndef __PSXACT_TIMER_CORE_HPP__
#define __PSXACT_TIMER_CORE_HPP__

#include "../system_core.hpp"

namespace psxact {
namespace timer {

  struct timer {
    uint16_t compare;
    uint16_t counter;
    bool running;

    // compare

    bool compare_reached;
    bool compare_irq_enable;
    bool compare_reset_counter;

    // maximum

    bool maximum_reached;
    bool maximum_irq_enable;

    // irq

    bool irq_repeat;
    bool irq_enable;
    bool irq_toggle;
    bool irq_bit;

    int clock_mode;
    int synch_enable;
    int synch_mode;

    int prescaler;
  };

  struct core {
    bool in_hblank;
    bool in_vblank;

    timer timers[3];

    uint32_t io_read(bus_width_t width, uint32_t address);

    void io_write(bus_width_t width, uint32_t address, uint32_t data);

    void tick();

    void hblank(bool active);

    void vblank(bool active);

  private:
    void timer_irq(int n);

    void timer_tick(int n);

    void timer_prescale_0();

    void timer_prescale_1();

    void timer_prescale_2();

    uint16_t timer_get_compare(int n);

    uint16_t timer_get_control(int n);

    uint16_t timer_get_counter(int n);

    void timer_set_compare(int n, uint16_t data);

    void timer_set_control(int n, uint16_t data);

    void timer_set_counter(int n, uint16_t data);
  };

}
}

#endif // __PSXACT_TIMER_CORE_HPP__
