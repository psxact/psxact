#include "timer/core.hpp"

#include <cassert>
#include <cstdio>
#include "args.hpp"
#include "timing.hpp"

// These are not implemented or partially implemented.
//
// Synchronization Modes for Counter 0:
//   0 = Pause counter during Hblank(s)
//   1 = Reset counter to 0000h at Hblank(s)
//   2 = Reset counter to 0000h at Hblank(s) and pause outside of Hblank
//   3 = Pause until Hblank occurs once, then switch to Free Run
// Synchronization Modes for Counter 1:
//   Same as above, but using Vblank instead of Hblank
// Synchronization Modes for Counter 2:
//   0 or 3 = Stop counter at current value (forever, no h/v-blank start)
//   1 or 2 = Free Run (same as when Synchronization Disabled)
//
// Counter 0: 1 or 3 = Dotclock
// Counter 1: 1 or 3 = Hblank
// Counter 2: 2 or 3 = System Clock/8
//
// PSX.256-pix Dotclock = CPU_FREQ * 11 / 70
// PSX.320-pix Dotclock = CPU_FREQ * 11 / 56
// PSX.368-pix Dotclock = CPU_FREQ * 11 / 49
// PSX.512-pix Dotclock = CPU_FREQ * 11 / 35
// PSX.640-pix Dotclock = CPU_FREQ * 11 / 28

using namespace psx::timer;
using namespace psx::util;

core::core(wire irq0, wire irq1, wire irq2)
  : addressable("timer", args::log_timer)
  , timers { timer(irq0), timer(irq1), timer(irq2) } {
}

void core::tick(int system) {
  system_over_8_prescale += system;
  int system_over_8 = system_over_8_prescale / 8;
  system_over_8_prescale &= 7;

  if (timer_get_source(0) == timer_source::system) {
    timer_run(0, system);
  }

  if (timer_get_source(1) == timer_source::system) {
    timer_run(1, system);
  }

  if (timer_get_source(2) == timer_source::system) {
    timer_run(2, system);
  } else if (timer_get_source(2) == timer_source::system_over_8) {
    timer_run(2, system_over_8);
  }
}

void core::enter_hblank() {
  in_hblank = true;
  timer_blanking_sync(0, in_hblank);

  if (timer_source(1) == timer_source::hblank) {
    timer_run(1, 1);
  }
}

void core::leave_hblank() {
  in_hblank = false;
  timer_blanking_sync(0, in_hblank);
}

void core::enter_vblank() {
  in_vblank = true;
  timer_blanking_sync(1, in_vblank);
}

void core::leave_vblank() {
  in_vblank = false;
  timer_blanking_sync(1, in_vblank);
}

void core::timer_run(int n, int amount) {
  auto &timer = timers[n];

  if (!timer.running || amount == 0) {
    return;
  }

  uint32_t counter = timer.counter + amount;
  uint32_t target;

  target = timer.counter_target + 1;

  if (timer.counter < target && counter >= target) {
    if ((timer.control & (1 << 3)) != 0) counter %= target;
    if ((timer.control & (1 << 4)) != 0) timer_irq(n);
    timer.control |= (1 << 11);
  }

  target = 0xffff + 1;

  if (timer.counter < target && counter > target) {
    if ((timer.control & (1 << 3)) == 0) counter %= target;
    if ((timer.control & (1 << 5)) != 0) timer_irq(n);
    timer.control |= (1 << 12);
  }

  timer.counter = uint16_t(counter);
}

void core::timer_irq(int n) {
  bool repeat = (timers[n].control & (1 <<  6)) != 0;
  bool toggle = (timers[n].control & (1 <<  7)) != 0;
  auto bit_10 = (timers[n].control & (1 << 10)) != 0;

  // Either toggle bit10 or clear it.
  if (toggle && repeat) {
    timer_irq_flag(n, !bit_10);
  } else {
    timer_irq_flag(n, 0);
  }

  if (!toggle && repeat) { // pulse mode, repeat
    timer_irq_flag(n, 1);
  }
}

void core::timer_irq_flag(int n, bool val) {
  if (val) {
    timers[n].irq(wire_state::off);
    timers[n].control |= (1 << 10);
  } else {
    timers[n].irq(wire_state::on);
    timers[n].control &= ~(1 << 10);
  }
}

timer_source core::timer_get_source(int n) {
  switch (n) {
    case 0: return (timers[0].control & (1 << 8)) ? timer_source::dotclock : timer_source::system;
    case 1: return (timers[1].control & (1 << 8)) ? timer_source::hblank : timer_source::system;
    case 2: return (timers[2].control & (1 << 9)) ? timer_source::system_over_8 : timer_source::system;
  }

  assert(0 && "Invalid value for parameter `n'");
}

void core::timer_blanking_sync(int n, bool active) {
  switch (timer_sync_mode(n)) {
    case timer_sync_mode::none:
      break;

    case timer_sync_mode::sync_mode_0: // 0 = Pause counter during blank(s)
      timers[n].running = !active;
      break;

    case timer_sync_mode::sync_mode_1: // 1 = Reset counter to 0000h at blank(s)
      if (active) {
        timers[n].counter = 0;
      }
      break;

    case timer_sync_mode::sync_mode_2: // 2 = Reset counter to 0000h at blank(s) and pause outside of blank
      timers[n].running = active;

      if (active) {
        timers[n].counter = 0;
      }
      break;

    case timer_sync_mode::sync_mode_3: // 3 = Pause until blank occurs once, then switch to Free Run
      if (active) {
        timers[n].running = true;
      }
      break;
  }
}

uint16_t core::timer_get_counter(int n) {
  return timers[n].counter;
}

uint16_t core::timer_get_control(int n) {
  uint16_t control = timers[n].control;

  timers[n].control &= ~(1 << 11); // Reset after reading
  timers[n].control &= ~(1 << 12); // Reset after reading

  return control;
}

uint16_t core::timer_get_counter_target(int n) {
  return timers[n].counter_target;
}

void core::timer_put_counter(int n, uint16_t val) {
  timers[n].counter = val;
}

void core::timer_put_control(int n, uint16_t val) {
  timers[n].counter  = 0;
  timers[n].running  = true;
  timers[n].control &= 0x1800;
  timers[n].control |= val & 0x3ff;
  timer_irq_flag(n, 1);

  if ((timers[n].control & 1) == 1) {
    uint32_t sync_mode = (timers[n].control >> 1) & 3;
    if (n == 0) {
      // TODO: do these sync modes take effect immediately, or on the next /HBL pulse?
      switch (sync_mode) {
        case 0: timers[0].running = !in_hblank; break; // 0 = Pause counter during Hblank(s)
        case 1: break; // 1 = Reset counter to 0000h at Hblank(s)
        case 2: timers[0].running = in_hblank; break; // 2 = Reset counter to 0000h at Hblank(s) and pause outside of Hblank
        case 3: timers[0].running = false; break; // 3 = Pause until Hblank occurs once, then switch to Free Run
      }
    }

    if (n == 1) {
      // TODO: do these sync modes take effect immediately, or on the next /VBL pulse?
      switch (sync_mode) {
        case 0: timers[1].running = !in_vblank; break; // 0 = Pause counter during Vblank(s)
        case 1: break; // 1 = Reset counter to 0000h at Vblank(s)
        case 2: timers[1].running = in_vblank; break; // 2 = Reset counter to 0000h at Vblank(s) and pause outside of Vblank
        case 3: timers[1].running = false; break; // 3 = Pause until Vblank occurs once, then switch to Free Run
      }
    }

    if (n == 2 && (sync_mode == 0 || sync_mode == 3)) {
      timers[n].running = false;
    }
  }
}

void core::timer_put_counter_target(int n, uint16_t val) {
  timers[n].counter_target = val;
}

uint32_t core::io_read(address_width width, uint32_t address) {
  timing::add_cpu_time(4);

  if (width == address_width::word || width == address_width::half) {
    int n = (address >> 4) & 3;

    switch (address & 15) {
      case 0: return timer_get_counter(n);
      case 4: return timer_get_control(n);
      case 8: return timer_get_counter_target(n);
    }
  }

  return addressable::io_read(width, address);
}

void core::io_write(address_width width, uint32_t address, uint32_t data) {
  timing::add_cpu_time(4);

  if (width == address_width::word || width == address_width::half) {
    int n = (address >> 4) & 3;

    switch (address & 15) {
      case 0: return timer_put_counter(n, data);
      case 4: return timer_put_control(n, data);
      case 8: return timer_put_counter_target(n, data);
    }
  }

  return addressable::io_write(width, address, data);
}
