#ifndef TIMER_CORE_HPP_
#define TIMER_CORE_HPP_

#include "interrupt-access.hpp"
#include "memory-component.hpp"

namespace psx::timer {

struct timer_t {
  uint16_t counter;
  bool running;

  struct {
    bool reached;
    bool irq_enable;
    bool reset_counter;
    uint16_t value;
  } compare;

  struct {
    bool reached;
    bool irq_enable;
  } maximum;

  struct {
    bool repeat;
    bool enable;
    bool toggle;
    bool bit;
  } irq;

  struct {
    int enable;
    int mode;
  } sync;

  struct {
    bool enable;
    int cycles;
    int single;
    int period;
  } prescaler;
};

class core_t : public memory_component_t {
  interrupt_access_t *irq;

  bool in_hblank;
  bool in_vblank;

  timer_t timers[3];

 public:
  explicit core_t(interrupt_access_t *irq, bool log_enabled);

  uint16_t io_read_half(uint32_t address);

  uint32_t io_read_word(uint32_t address);

  void io_write_half(uint32_t address, uint16_t data);

  void io_write_word(uint32_t address, uint32_t data);

  void tick(int amount);

  void hblank(bool active);

  void vblank(bool active);

 private:
  void unit_init(int n, int single, int period);

  void unit_irq(int n);

  void unit_tick(int n);

  void unit_prescale(int n, int amount);

  uint16_t unit_get_compare(int n);

  uint16_t unit_get_control(int n);

  uint16_t unit_get_counter(int n);

  void unit_set_compare(int n, uint16_t data);

  void unit_set_control(int n, uint16_t data);

  void unit_set_counter(int n, uint16_t data);
};

}  // namespace psx::timer

#endif  // TIMER_CORE_HPP_
