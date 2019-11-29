// Copyright 2018 psxact

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

#include "timer/core.hpp"

#include "utility.hpp"

using psx::timer::core_t;

core_t::core_t(interrupt_access_t *irq)
  : memory_component_t("counter")
  , irq(irq) {
  unit_init(0, 11, 7 * 4);
  unit_init(1, 11, 7 * 3413);
  unit_init(2,  1, 8);
}

uint32_t core_t::io_read_half(uint32_t address) {
  auto m = (address >> 2) & 3;
  auto n = (address >> 4) & 3;

  if (n == 3) {
    return 0;
  }

  switch (m) {
    case  0: return unit_get_counter(n);
    case  1: return unit_get_control(n);
    case  2: return unit_get_compare(n);
    default: return 0;
  }
}

uint32_t core_t::io_read_word(uint32_t address) {
  return io_read_half(address);
}

void core_t::io_write_half(uint32_t address, uint32_t data) {
  auto m = (address >> 2) & 3;
  auto n = (address >> 4) & 3;

  if (n == 3) {
    return;
  }

  switch (m) {
    case  0: return unit_set_counter(n, data);
    case  1: return unit_set_control(n, data);
    case  2: return unit_set_compare(n, data);
    default: return;
  }
}

void core_t::io_write_word(uint32_t address, uint32_t data) {
  io_write_half(address, data);
}

uint16_t core_t::unit_get_compare(int n) {
  return timers[n].compare.value;
}

uint16_t core_t::unit_get_control(int n) {
  auto &timer = timers[n];

  auto control =
    (timer.sync.enable << 0) |
    (timer.sync.mode << 1) |
    (timer.compare.reset_counter << 3) |
    (timer.compare.irq_enable << 4) |
    (timer.maximum.irq_enable << 5) |
    (timer.irq.repeat << 6) |
    (timer.irq.toggle << 7) |
    (timer.prescaler.enable << 8) |
    (timer.irq.bit << 10) |
    (timer.compare.reached << 11) |
    (timer.maximum.reached << 12);

  timer.compare.reached = 0;
  timer.maximum.reached = 0;

  return uint16_t(control);
}

uint16_t core_t::unit_get_counter(int n) {
  return timers[n].counter;
}

static bool is_running(int sync_mode, bool b = 1) {
  switch (sync_mode) {
    case 0: return b == 0;
    case 1: return 1 == 1;
    case 2: return b == 1;
    case 3: return 1 == 0;
  }

  return 0;
}

void core_t::unit_init(int n, int single, int period) {
  auto &prescaler = timers[n].prescaler;

  prescaler.single = single;
  prescaler.period = period;
  prescaler.cycles = period;
}

void core_t::unit_set_control(int n, uint16_t data) {
  auto &timer = timers[n];

  timer.counter = 0;
  timer.irq.enable = 1;
  timer.irq.bit = 1;

  timer.sync.enable = (data >> 0) & 1;
  timer.sync.mode = (data >> 1) & 3;
  timer.compare.reset_counter = (data >> 3) & 1;
  timer.compare.irq_enable = (data >> 4) & 1;
  timer.maximum.irq_enable = (data >> 5) & 1;
  timer.irq.repeat = (data >> 6) & 1;
  timer.irq.toggle = (data >> 7) & 1;
  timer.prescaler.enable = n == 2
    ? (data >> 9) & 1
    : (data >> 8) & 1;

  if (timer.sync.enable == 0) {
    timer.running = 1;
  } else if (n == 0) {
    timer.running = is_running(timer.sync.mode, in_hblank);
  } else if (n == 1) {
    timer.running = is_running(timer.sync.mode, in_vblank);
  } else if (n == 2) {
    timer.running = is_running(timer.sync.mode);
  }
}

void core_t::unit_set_compare(int n, uint16_t data) {
  timers[n].compare.value = data;
}

void core_t::unit_set_counter(int n, uint16_t data) {
  timers[n].counter = data;
}

void core_t::unit_irq(int n) {
  auto &interrupt = timers[n].irq;

  if (interrupt.enable) {
    interrupt.enable = interrupt.repeat;
    interrupt.bit = interrupt.toggle
      ? 1 - interrupt.bit
      : 0;

    if (interrupt.bit == 0) {
      switch (n) {
        case 0: return irq->send(interrupt_type_t::TMR0);
        case 1: return irq->send(interrupt_type_t::TMR1);
        case 2: return irq->send(interrupt_type_t::TMR2);
      }
    }
  }
}

void core_t::unit_tick(int n) {
  auto &timer = timers[n];

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

void core_t::unit_prescale(int n, int amount) {
  auto &prescaler = timers[n].prescaler;

  if (prescaler.enable == 0) {
    for (int i = 0; i < amount; i++) {
      unit_tick(n);
    }
  } else {
    prescaler.cycles -= prescaler.single * amount;

    while (prescaler.cycles <= 0) {
      prescaler.cycles += prescaler.period;
      unit_tick(n);
    }
  }
}

void core_t::tick(int amount) {
  unit_prescale(0, amount);
  unit_prescale(1, amount);
  unit_prescale(2, amount);
}

void core_t::hblank(bool active) {
  in_hblank = active;

  auto &timer = timers[0];

  timer.running = is_running(timer.sync.mode, in_hblank);
}

void core_t::vblank(bool active) {
  in_vblank = active;

  auto &timer = timers[1];

  timer.running = is_running(timer.sync.mode, in_vblank);
}
