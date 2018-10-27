#ifndef __psxact_counter__
#define __psxact_counter__


#include "console.hpp"
#include "interrupt-access.hpp"
#include "memory-component.hpp"


namespace psx {

struct timer_t {
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

  int sync_enable;
  int sync_mode;

  struct {
    bool enable;
    int cycles;
    int single;
    int period;
  } prescaler;
};


class timer_unit_t : public memory_component_t {

  interrupt_access_t *irq;

  bool in_hblank;
  bool in_vblank;

  timer_t units[3];

public:

  timer_unit_t(interrupt_access_t *irq);

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

}

#endif // __psxact_counter__
