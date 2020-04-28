#include "timer/core.hpp"

#include <cassert>

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

core_t::core_t(interruptible_t &irq, bool log_enabled)
  : addressable_t("timer", log_enabled)
  , irq(irq) {
}

void core_t::run(int amount) {
  prescale_system_over_8 += amount;
  int system_over_8 = prescale_system_over_8 / 8;
  prescale_system_over_8 &= 7;

  timer_run(0, amount, system_over_8);
  timer_run(1, amount, system_over_8);
  timer_run(2, amount, system_over_8);
}

void core_t::timer_run(int n, int system, int system_over_8) {
  auto &timer = timers[n];

  uint32_t control = uint32_t(timer.control);
  if (control & 1) {
    // 0     Synchronization Enable (0=Free Run, 1=Synchronize via Bit1-2)
    // 1-2   Synchronization Mode   (0-3, see lists below)
    assert(0 && "Sync modes aren't supported yet.");
  }

  uint32_t counter = timer.counter;

  switch (timer_source(n)) {
    case timer_source_t::system:
      counter += system;
      break;

    case timer_source_t::system_over_8:
      counter += system;
      break;

    case timer_source_t::hblank:
      assert(0 && "HBlank isn't supported yet.");
      break;

    case timer_source_t::dotclock:
      assert(0 && "Dotclock isn't supported yet.");
      break;
  }

  uint32_t target;

  target = timer.counter_target;

  if (timer.counter < target && counter >= target) {
    if ((timer.control & (1 << 3)) == 0) counter %= target;
    if ((timer.control & (1 << 4)) != 0) timer_irq(n); // 4     IRQ when Counter=Target (0=Disable, 1=Enable)
    timer.control |= (1 << 11);
  }

  target = 0x10000;

  if (timer.counter < target && counter > target) {
    if ((timer.control & (1 << 3)) != 0) counter %= target;
    if ((timer.control & (1 << 5)) != 0) timer_irq(n); // 5     IRQ when Counter=FFFFh  (0=Disable, 1=Enable)
    timer.control |= (1 << 12);
  }

  timer.counter = counter;
}

void core_t::timer_irq(int n) {
  auto &timer = timers[n];

  bool repeat = (timer.control & (1 <<  6)) != 0;
  bool toggle = (timer.control & (1 <<  7)) != 0;
  auto old_10 = (timer.control & (1 << 10)) != 0;
  auto new_10 = old_10;

  // Either toggle bit10 or clear it.
  if (toggle && repeat) {
    new_10 = !old_10;
  } else {
    new_10 = false;
  }

  if (old_10 && !new_10) {
    log("Sending interrupt for timer %d", n);

    // Interrupts are enabled, and we had a falling-edge of bit10.
    switch (n) {
      case 0: irq.interrupt(interrupt_type_t::timer0); break;
      case 1: irq.interrupt(interrupt_type_t::timer1); break;
      case 2: irq.interrupt(interrupt_type_t::timer2); break;
    }
  }

  if (!toggle && repeat) { // pulse mode, repeat
    new_10 = true;
  }

  timers[n].control &= ~(1 << 10);
  timers[n].control |=  (1 << 10);
}

timer_source_t core_t::timer_source(int n) {
  switch (n) {
    case 0: return (timers[0].control & (1 << 8)) ? timer_source_t::dotclock : timer_source_t::system;
    case 1: return (timers[1].control & (1 << 8)) ? timer_source_t::hblank : timer_source_t::system;
    case 2: return (timers[2].control & (1 << 9)) ? timer_source_t::system_over_8 : timer_source_t::system;
  }
  
  assert(0 && "Invalid value for parameter `n'");
}

uint16_t core_t::timer_get_counter(int n) {
  return timers[n].counter;
}

uint16_t core_t::timer_get_control(int n) {
  uint16_t control = timers[n].control;

  timers[n].control &= ~(1 << 11); // Reset after reading
  timers[n].control &= ~(1 << 12); // Reset after reading

  return control;
}

uint16_t core_t::timer_get_counter_target(int n) {
  return timers[n].counter_target;
}

void core_t::timer_put_counter(int n, uint16_t val) {
  timers[n].counter = val;
}

void core_t::timer_put_control(int n, uint16_t val) {
  timers[n].control &= 0x1800;
  timers[n].control |= val & 0x3ff;
  timers[n].control |= 0x400;
}

void core_t::timer_put_counter_target(int n, uint16_t val) {
  timers[n].counter_target = val;
}

uint16_t core_t::io_read_half(uint32_t address) {
  int n = (address >> 4) & 3;

  switch (address & 0x1F80110F) {
    case 0x1F801100: return timer_get_counter(n);
    case 0x1F801104: return timer_get_control(n);
    case 0x1F801108: return timer_get_counter_target(n);
  }

  assert(0 && "Unhandled timer address");
}

uint32_t core_t::io_read_word(uint32_t address) {
  return io_read_half(address);
}

void core_t::io_write_half(uint32_t address, uint16_t data) {
  int n = (address >> 4) & 3;

  switch (address & 0x1F80110F) {
    case 0x1F801100: return timer_put_counter(n, data);
    case 0x1F801104: return timer_put_control(n, data);
    case 0x1F801108: return timer_put_counter_target(n, data);
  }

  assert(0 && "Unhandled timer address");
}
