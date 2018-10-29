// Copyright 2018 psxact

#ifndef TIMER_CORE_HPP_
#define TIMER_CORE_HPP_


#include "console.hpp"
#include "interrupt-access.hpp"
#include "memory-component.hpp"


namespace psx {
namespace timer {

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

  int sync_enable;
  int sync_mode;

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
  explicit core_t(interrupt_access_t *irq);

  uint32_t io_read_half(uint32_t address);

  uint32_t io_read_word(uint32_t address);

  void io_write_half(uint32_t address, uint32_t data);

  void io_write_word(uint32_t address, uint32_t data);

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

}  // namespace timer
}  // namespace psx

#endif  // TIMER_CORE_HPP_