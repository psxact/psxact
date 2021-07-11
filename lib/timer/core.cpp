#include "timer/core.hpp"

#include <cstdio>

#include "util/panic.hpp"
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

core::core(opts &o, wire irq0, wire irq1, wire irq2)
  : addressable(o, component::timer)
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

  if (timer_get_source(1) == timer_source::hblank) {
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

  while (1) {
    auto cycleEdge = 65536 - timer.counter;
    if (amount >= cycleEdge) {
      amount -= cycleEdge;
      timer_run_real(n, cycleEdge);
    }
    else {
      timer_run_real(n, amount);
      return;
    }
  }
}

void core::timer_run_real(int n, int amount) {
  auto &timer = timers[n];

  if (!timer.running || amount == 0) {
    return;
  }

  auto counter = int32_t(timer.counter) + amount;
  auto control = int32_t(timer.control);
  auto maximum = int32_t(timer.maximum);

  if (counter > maximum && timer.counter <= timer.maximum) {
    control |= (1 << 11);

    if (control & (1 << 3)) {
      counter %= maximum + 1;
    }
  }

  if (counter > 0xffff) {
    control |= (1 << 12);
    counter &= 0xffff;
  }

  timer.control = uint16_t(control);
  timer.counter = uint16_t(counter);
}

void core::timer_irq(int n) {
  auto repeat = (timers[n].control & (1 << 6)) != 0;
  auto toggle = (timers[n].control & (1 << 7)) != 0;

  if (toggle) {
    if (timers[n].control & (1 << 10)) {
      timer_irq_real(n, timer_irq_flag::active);
    } else {
      timer_irq_real(n, timer_irq_flag::inactive);
    }
  } else {
    timer_irq_real(n, timer_irq_flag::active);

    if (repeat) {
      timer_irq_real(n, timer_irq_flag::inactive);
    }
  }

  timers[n].inhibit = !repeat;
}

void core::timer_irq_real(int n, timer_irq_flag val) {
  if (timers[n].inhibit) {
    // TODO: does the IRQ flag still toggle and just IRQs are suppressed, or does bit10 stay 1?
    timers[n].irq(wire_state::off);
  } else {
    if (val == timer_irq_flag::active) {
      timers[n].control &= ~(1 << 10);
      timers[n].irq(wire_state::on);
    } else {
      timers[n].control |= (1 << 10);
      timers[n].irq(wire_state::off);
    }
  }
}

timer_source core::timer_get_source(int n) {
  switch (n) {
    case 0: return (timers[0].control & (1 << 8)) ? timer_source::dotclock : timer_source::system;
    case 1: return (timers[1].control & (1 << 8)) ? timer_source::hblank : timer_source::system;
    case 2: return (timers[2].control & (1 << 9)) ? timer_source::system_over_8 : timer_source::system;
  }

  PANIC("Invalid value for parameter `n'");
	return timer_source::system;
}

timer_sync_mode core::timer_get_sync_mode(int n) {
	if ((timers[n].control & (1 << 0)) == 0) {
		return timer_sync_mode::none;
	}

	switch ((timers[n].control >> 1) & 3) {
		case  0: return timer_sync_mode::sync_mode_0;
		case  1: return timer_sync_mode::sync_mode_1;
		case  2: return timer_sync_mode::sync_mode_2;
		default: return timer_sync_mode::sync_mode_3;
	}
}

void core::timer_blanking_sync(int n, bool active) {
  switch (timer_get_sync_mode(n)) {
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

uint16_t core::timer_get_maximum(int n) {
  return timers[n].maximum;
}

void core::timer_put_counter(int n, uint16_t val) {
  timers[n].counter = val;
}

void core::timer_put_control(int n, uint16_t val) {
  timers[n].counter  = 0;
  timers[n].inhibit  = false;
  timers[n].running  = true;
  timers[n].control &= 0x1800;
  timers[n].control |= val & 0x3ff;
  timer_irq_real(n, timer_irq_flag::inactive);

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

void core::timer_put_maximum(int n, uint16_t val) {
  timers[n].maximum = val;
}

uint32_t core::io_read(address_width width, uint32_t address) {
  timing::add_cpu_time(4);

  if (width == address_width::word || width == address_width::half) {
    int n = (address >> 4) & 3;

    switch (address & 15) {
      case 0: return timer_get_counter(n);
      case 4: return timer_get_control(n);
      case 8: return timer_get_maximum(n);
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
      case 8: return timer_put_maximum(n, data);
    }
  }

  return addressable::io_write(width, address, data);
}
