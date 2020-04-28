#ifndef TIMER_CORE_HPP_
#define TIMER_CORE_HPP_

#include "addressable.hpp"
#include "interruptible.hpp"

namespace psx::timer {

  enum class timer_source_t {
    system,
    system_over_8,
    hblank,
    dotclock
  };

  struct timer_t final {
    uint16_t counter { 0 };
    uint16_t control { 0 };
    uint16_t counter_target { 0 };
  };

  class core_t final : public addressable_t {
    interruptible_t &irq;
    timer_t timers[3] = {};
    int prescale_system_over_8 = {};

  public:
    explicit core_t(interruptible_t &irq, bool log_enabled);

    void run(int amount);

    uint16_t io_read_half(uint32_t address);
    uint32_t io_read_word(uint32_t address);
    void io_write_half(uint32_t address, uint16_t data);

  private:
    void timer_run(int n, int system, int system_over_8);
    void timer_irq(int n);

    uint16_t timer_get_counter(int n);
    uint16_t timer_get_control(int n);
    uint16_t timer_get_counter_target(int n);

    void timer_put_counter(int n, uint16_t val);
    void timer_put_control(int n, uint16_t val);
    void timer_put_counter_target(int n, uint16_t val);

    timer_source_t timer_source(int n);
  };
}  // namespace psx::timer

#endif  // TIMER_CORE_HPP_
