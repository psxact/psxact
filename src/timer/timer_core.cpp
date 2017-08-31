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

#include "timer_core.hpp"
#include "../utility.hpp"

using namespace psxact;
using namespace psxact::timer;

uint32_t core::io_read(bus_width_t width, uint32_t address) {
  if (utility::log_timer) {
    printf("timer::io_read(%d, 0x%08x)\n", width, address);
  }

  switch (address & ~3) {
  case 0x1f801100: return timer_get_counter(0);
  case 0x1f801104: return timer_get_control(0);
  case 0x1f801108: return timer_get_compare(0);
  case 0x1f80110c: return 0;

  case 0x1f801110: return timer_get_counter(1);
  case 0x1f801114: return timer_get_control(1);
  case 0x1f801118: return timer_get_compare(1);
  case 0x1f80111c: return 0;

  case 0x1f801120: return timer_get_counter(2);
  case 0x1f801124: return timer_get_control(2);
  case 0x1f801128: return timer_get_compare(2);
  case 0x1f80112c: return 0;

  case 0x1f801130: return 0;
  case 0x1f801134: return 0;
  case 0x1f801138: return 0;
  case 0x1f80113c: return 0;
  }

  return 0;
}

void core::io_write(bus_width_t width, uint32_t address, uint32_t data) {
  if (utility::log_timer) {
    printf("timer::io_write(%d, 0x%08x, 0x%08x)\n", width, address, data);
  }

  switch (address & ~3) {
  case 0x1f801100: return timer_set_counter(0, data);
  case 0x1f801104: return timer_set_control(0, data);
  case 0x1f801108: return timer_set_compare(0, data);
  case 0x1f80110c: return;

  case 0x1f801110: return timer_set_counter(1, data);
  case 0x1f801114: return timer_set_control(1, data);
  case 0x1f801118: return timer_set_compare(1, data);
  case 0x1f80111c: return;

  case 0x1f801120: return timer_set_counter(2, data);
  case 0x1f801124: return timer_set_control(2, data);
  case 0x1f801128: return timer_set_compare(2, data);
  case 0x1f80112c: return;

  case 0x1f801130: return;
  case 0x1f801134: return;
  case 0x1f801138: return;
  case 0x1f80113c: return;
  }
}

uint16_t core::timer_get_compare(int n) {
  return timers[n].compare;
}

uint16_t core::timer_get_control(int n) {
  auto &timer = timers[n];

  auto control =
    (timer.synch_enable << 0) |
    (timer.synch_mode << 1) |
    (timer.compare_reset_counter << 3) |
    (timer.compare_irq_enable << 4) |
    (timer.maximum_irq_enable << 5) |
    (timer.irq_repeat << 6) |
    (timer.irq_toggle << 7) |
    (timer.clock_mode << 8) |
    (timer.irq_bit << 10) |
    (timer.compare_reached << 11) |
    (timer.maximum_reached << 12);

  timer.compare_reached = 0;
  timer.maximum_reached = 0;

  return uint16_t(control);
}

uint16_t core::timer_get_counter(int n) {
  return timers[n].counter;
}

static int decode_irq_compare_enable(uint16_t data) {
  return (data >> 4) & 1;
}

static int decode_irq_maximum_enable(uint16_t data) {
  return (data >> 5) & 1;
}

static int decode_irq_repeat(uint16_t data) {
  return (data >> 6) & 1;
}

static int decode_irq_toggle(uint16_t data) {
  return (data >> 7) & 1;
}

static int decode_compare_reset_counter(uint16_t data) {
  return (data >> 3) & 1;
}

static int decode_clock_mode(uint16_t data) {
  return (data >> 8) & 3;
}

static int decode_synch_enable(uint16_t data) {
  return (data >> 0) & 1;
}

static int decode_synch_mode(uint16_t data) {
  return (data >> 1) & 3;
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

void core::timer_set_control(int n, uint16_t data) {
  auto &timer = timers[n];

  timer.counter = 0;
  timer.irq_enable = 1;
  timer.irq_bit = 1;

  timer.clock_mode = decode_clock_mode(data);
  timer.compare_irq_enable = decode_irq_compare_enable(data);
  timer.maximum_irq_enable = decode_irq_maximum_enable(data);
  timer.irq_repeat = decode_irq_repeat(data);
  timer.irq_toggle = decode_irq_toggle(data);
  timer.compare_reset_counter = decode_compare_reset_counter(data);
  timer.synch_enable = decode_synch_enable(data);
  timer.synch_mode = decode_synch_mode(data);

  if (timer.synch_enable == 0) {
    timer.running = 1;
  }
  else if (n == 0) {
    timer.running = is_running(timer.synch_mode, in_hblank);
  }
  else if (n == 1) {
    timer.running = is_running(timer.synch_mode, in_vblank);
  }
  else if (n == 2) {
    timer.running = is_running(timer.synch_mode);
  }
}

void core::timer_set_compare(int n, uint16_t data) {
  timers[n].compare = data;
}

void core::timer_set_counter(int n, uint16_t data) {
  timers[n].counter = data;
}

void core::timer_irq(int n) {
  auto &timer = timers[n];

  if (timer.irq_enable) {
    timer.irq_enable = timer.irq_repeat;

    if (timer.irq_toggle && timer.irq_bit == 0) {
      timer.irq_bit = 1;
    }
    else {
      timer.irq_bit = 0;
      system->irq(4 + n);
    }
  }
}

void core::timer_tick(int n) {
  auto &timer = timers[n];

  if (timer.running) {
    timer.counter++;

    if (timer.counter == 0) {
      timer.maximum_reached = 1;

      if (timer.maximum_irq_enable) {
        timer_irq(n);
      }
    }

    if (timer.counter == timer.compare) {
      timer.compare_reached = 1;

      if (timer.compare_irq_enable) {
        timer_irq(n);
      }

      if (timer.compare_reset_counter) {
        timer.counter = 0;
      }
    }
  }
}

void core::timer_0_prescale() {
  auto &timer = timers[0];

  if (timer.clock_mode == 0 || timer.clock_mode == 2) {
    timer_tick(0);
  }
  else {
    const int single = 11;
    const int period = 7 * 4;

    timer.prescaler += single;

    while (timer.prescaler >= period) {
      timer.prescaler -= period;
      timer_tick(0);
    }
  }
}

void core::timer_1_prescale() {
  auto &timer = timers[1];

  if (timer.clock_mode == 0 || timer.clock_mode == 2) {
    timer_tick(1);
  }
  else {
    const int single = 11;
    const int period = 7 * 3413;

    timer.prescaler += single;

    while (timer.prescaler >= period) {
      timer.prescaler -= period;
      timer_tick(1);
    }
  }
}

void core::timer_2_prescale() {
  auto &timer = timers[2];

  if (timer.clock_mode == 0 || timer.clock_mode == 1) {
    timer_tick(2);
  }
  else {
    const int single = 1;
    const int period = 8;

    timer.prescaler += single;

    while (timer.prescaler >= period) {
      timer.prescaler -= period;
      timer_tick(2);
    }
  }
}

void core::tick() {
  timer_0_prescale();
  timer_1_prescale();
  timer_2_prescale();
}

void core::hblank(bool active) {
  in_hblank = active;

  auto &timer = timers[0];

  timer.running = is_running(timer.synch_mode, in_hblank);
}

void core::vblank(bool active) {
  in_vblank = active;

  auto &timer = timers[1];

  timer.running = is_running(timer.synch_mode, in_vblank);
}
