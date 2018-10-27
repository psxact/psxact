/**
 * Unimplemented
 *
 * - Timer 0
 *   - Horizontal blanking behavior
 *   - Pixel clock input: Timing is fixed for 640px clock
 *
 * - Timer 1
 *   - Vertical blanking behavior
 *   - Horizontal clock input
 */

#include "timer/timer.hpp"

#include "utility.hpp"


using namespace psx;

timer_unit_t::timer_unit_t(interrupt_access_t *irq)
  : memory_component_t("counter")
  , irq(irq) {

  unit_init(0, 11, 7 * 4);
  unit_init(1, 11, 7 * 3413);
  unit_init(2,  1, 8);
}


uint32_t timer_unit_t::io_read_half(uint32_t address) {
  switch (address & ~3) {
    case 0x1f801100: return unit_get_counter(0);
    case 0x1f801104: return unit_get_control(0);
    case 0x1f801108: return unit_get_compare(0);
    case 0x1f80110c: return 0;

    case 0x1f801110: return unit_get_counter(1);
    case 0x1f801114: return unit_get_control(1);
    case 0x1f801118: return unit_get_compare(1);
    case 0x1f80111c: return 0;

    case 0x1f801120: return unit_get_counter(2);
    case 0x1f801124: return unit_get_control(2);
    case 0x1f801128: return unit_get_compare(2);
    case 0x1f80112c: return 0;

    case 0x1f801130: return 0;
    case 0x1f801134: return 0;
    case 0x1f801138: return 0;
    case 0x1f80113c: return 0;
  }

  return 0;
}


uint32_t timer_unit_t::io_read_word(uint32_t address) {
  return io_read_half(address);
}


void timer_unit_t::io_write_half(uint32_t address, uint32_t data) {
  switch (address & ~3) {
    case 0x1f801100: return unit_set_counter(0, data);
    case 0x1f801104: return unit_set_control(0, data);
    case 0x1f801108: return unit_set_compare(0, data);
    case 0x1f80110c: return;

    case 0x1f801110: return unit_set_counter(1, data);
    case 0x1f801114: return unit_set_control(1, data);
    case 0x1f801118: return unit_set_compare(1, data);
    case 0x1f80111c: return;

    case 0x1f801120: return unit_set_counter(2, data);
    case 0x1f801124: return unit_set_control(2, data);
    case 0x1f801128: return unit_set_compare(2, data);
    case 0x1f80112c: return;

    case 0x1f801130: return;
    case 0x1f801134: return;
    case 0x1f801138: return;
    case 0x1f80113c: return;
  }
}


void timer_unit_t::io_write_word(uint32_t address, uint32_t data) {
  io_write_half(address, data);
}


uint16_t timer_unit_t::unit_get_compare(int n) {
  return units[n].compare.value;
}


uint16_t timer_unit_t::unit_get_control(int n) {
  auto &unit = units[n];

  auto control =
    (unit.sync_enable << 0) |
    (unit.sync_mode << 1) |
    (unit.compare.reset_counter << 3) |
    (unit.compare.irq_enable << 4) |
    (unit.maximum.irq_enable << 5) |
    (unit.irq.repeat << 6) |
    (unit.irq.toggle << 7) |
    (unit.prescaler.enable << 8) |
    (unit.irq.bit << 10) |
    (unit.compare.reached << 11) |
    (unit.maximum.reached << 12);

  unit.compare.reached = 0;
  unit.maximum.reached = 0;

  return uint16_t(control);
}


uint16_t timer_unit_t::unit_get_counter(int n) {
  return units[n].counter;
}


static bool is_running(int synch_mode, bool b = 1) {
  switch (synch_mode) {
    case 0: return b == 0;
    case 1: return 1 == 1;
    case 2: return b == 1;
    case 3: return 1 == 0;
  }

  return 0;
}


void timer_unit_t::unit_init(int n, int single, int period) {
  units[n].prescaler.single = single;
  units[n].prescaler.period = period;
  units[n].prescaler.cycles = period;
}


void timer_unit_t::unit_set_control(int n, uint16_t data) {
  auto &unit = units[n];

  unit.counter = 0;
  unit.irq.enable = 1;
  unit.irq.bit = 1;

  unit.sync_enable = (data >> 0) & 1;
  unit.sync_mode = (data >> 1) & 3;
  unit.compare.reset_counter = (data >> 3) & 1;
  unit.compare.irq_enable = (data >> 4) & 1;
  unit.maximum.irq_enable = (data >> 5) & 1;
  unit.irq.repeat = (data >> 6) & 1;
  unit.irq.toggle = (data >> 7) & 1;
  unit.prescaler.enable = n == 2
    ? (data >> 9) & 1
    : (data >> 8) & 1
    ;

  if (unit.sync_enable == 0) {
    unit.running = 1;
  }
  else if (n == 0) {
    unit.running = is_running(unit.sync_mode, in_hblank);
  }
  else if (n == 1) {
    unit.running = is_running(unit.sync_mode, in_vblank);
  }
  else if (n == 2) {
    unit.running = is_running(unit.sync_mode);
  }
}


void timer_unit_t::unit_set_compare(int n, uint16_t data) {
  units[n].compare.value = data;
}


void timer_unit_t::unit_set_counter(int n, uint16_t data) {
  units[n].counter = data;
}


void timer_unit_t::unit_irq(int n) {
  auto &interrupt = units[n].irq;

  if (interrupt.enable) {
    interrupt.enable = interrupt.repeat;
    interrupt.bit = interrupt.toggle
      ? 1 - interrupt.bit
      : 0
      ;

    if (interrupt.bit == 0) {
      switch (n) {
        case 0: return irq->send(interrupt_type_t::TMR0);
        case 1: return irq->send(interrupt_type_t::TMR1);
        case 2: return irq->send(interrupt_type_t::TMR2);
      }
    }
  }
}


void timer_unit_t::unit_tick(int n) {
  auto &timer = units[n];

  if (timer.running) {
    timer.counter++;

    if (timer.counter == 0) {
      timer.maximum.reached = 1;

      if (timer.maximum.irq_enable) {
        unit_irq(n);
      }
    }

    if (timer.counter == timer.compare.value) {
      timer.compare.reached = 1;

      if (timer.compare.irq_enable) {
        unit_irq(n);
      }

      if (timer.compare.reset_counter) {
        timer.counter = 0;
      }
    }
  }
}


void timer_unit_t::unit_prescale(int n) {
  auto &prescaler = units[n].prescaler;

  if (prescaler.enable == 0) {
    unit_tick(n);
  }
  else {
    prescaler.cycles -= prescaler.single;

    while (prescaler.cycles <= 0) {
      prescaler.cycles += prescaler.period;
      unit_tick(n);
    }
  }
}


void timer_unit_t::tick() {
  unit_prescale(0);
  unit_prescale(1);
  unit_prescale(2);
}


void timer_unit_t::hblank(bool active) {
  in_hblank = active;

  auto &unit = units[0];

  unit.running = is_running(unit.sync_mode, in_hblank);
}


void timer_unit_t::vblank(bool active) {
  in_vblank = active;

  auto &unit = units[1];

  unit.running = is_running(unit.sync_mode, in_vblank);
}
