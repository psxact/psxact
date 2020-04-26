#ifndef TIMER_CORE_HPP_
#define TIMER_CORE_HPP_

#include "addressable.hpp"
#include "interruptible.hpp"

namespace psx::timer {

  struct timer_t final {
    uint16_t counter;
    uint16_t control;
    uint16_t counter_target;
    bool irq_enable;
  };

  class core_t final : public addressable_t {
    interruptible_t *irq;
    timer_t timers[3];

  public:
    explicit core_t(interruptible_t *irq, bool log_enabled);

    void run(int amount);

    uint16_t io_read_half(uint32_t address);
    uint32_t io_read_word(uint32_t address);
    void io_write_half(uint32_t address, uint16_t data);

  private:
    void timer_run(int n, int amount);
    void timer_irq(int n);
  };
}  // namespace psx::timer

#endif  // TIMER_CORE_HPP_
