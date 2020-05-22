#ifndef TIMER_CORE_HPP_
#define TIMER_CORE_HPP_

#include "util/wire.hpp"
#include "addressable.hpp"

namespace psx::timer {

  enum class timer_source {
    system,
    system_over_8,
    hblank,
    dotclock
  };

  enum class timer_sync_mode {
    none,
    sync_mode_0,
    sync_mode_1,
    sync_mode_2,
    sync_mode_3
  };

  struct timer final {
    uint16_t counter = {};
    uint16_t control = {};
    uint16_t counter_target = {};
    bool running = {};

    util::wire irq;

    timer(util::wire irq) : irq(irq) {}
  };

  class core final : public addressable {
    timer timers[3];
    int system_over_8_prescale = {};
    bool in_hblank = {};
    bool in_vblank = {};

  public:
    explicit core(util::wire irq0, util::wire irq1, util::wire irq2);

    void tick(int amount);

    uint32_t io_read(address_width width, uint32_t address);
    void io_write(address_width width, uint32_t address, uint32_t data);

    void enter_hblank();
    void leave_hblank();

    void enter_vblank();
    void leave_vblank();

  private:
    void timer_run(int n, int amount);
    void timer_irq(int n);
    void timer_irq_flag(int n, bool val);

    uint16_t timer_get_counter(int n);
    uint16_t timer_get_control(int n);
    uint16_t timer_get_counter_target(int n);

    void timer_put_counter(int n, uint16_t val);
    void timer_put_control(int n, uint16_t val);
    void timer_put_counter_target(int n, uint16_t val);

    timer_source timer_get_source(int n);
    timer_sync_mode timer_get_sync_mode(int n);
    void timer_blanking_sync(int n, bool active);
  };
}

#endif
