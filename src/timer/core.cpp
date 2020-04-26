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
// Counter 1:  0 or 2 = System Clock,  1 or 3 = Hblank
// Counter 2:  0 or 1 = System Clock,  2 or 3 = System Clock/8
//
// PSX.256-pix Dotclock = * 11 / 70
// PSX.320-pix Dotclock = * 11 / 56
// PSX.368-pix Dotclock = * 11 / 49
// PSX.512-pix Dotclock = * 11 / 35
// PSX.640-pix Dotclock = * 11 / 28

using namespace psx::timer;

core_t::core_t(interruptible_t *irq, bool log_enabled)
  : addressable_t("timer", log_enabled)
  , irq(irq) {
}

void core_t::run(int amount) {
  timer_run(0, amount);
  timer_run(1, amount);
  timer_run(2, amount);
}

void core_t::timer_run(int n, int amount) {
  auto &timer = timers[n];

  uint32_t control = uint32_t(timer.control);
  if (control & 1) {
    // 0     Synchronization Enable (0=Free Run, 1=Synchronize via Bit1-2)
    // 1-2   Synchronization Mode   (0-3, see lists below)
    assert(0 && "Sync modes aren't supported yet.");
  }

  uint32_t counter = timer.counter;

  switch ((control >> 8) & 3) { // Counter 0:  0 or 2 = System Clock,  1 or 3 = Dotclock
    case 0: case 2: counter += amount; break;
    case 1: case 3: assert(0 && "Dotclock isn't supported yet."); break;
  }

  uint32_t target = timer.counter_target;

  if (timer.counter < target && counter >= target) {
    if ((timer.control & (1 << 3)) == 0) counter %= target;
    if ((timer.control & (1 << 4)) != 0) timer_irq(n); // 4     IRQ when Counter=Target (0=Disable, 1=Enable)
    timer.control |= (1 << 11);
  }

  if (timer.counter < 0xffff && counter >= 0xffff) {
    if ((timer.control & (1 << 3)) != 0) counter %= 0xffff;
    if ((timer.control & (1 << 5)) != 0) timer_irq(n); // 5     IRQ when Counter=FFFFh  (0=Disable, 1=Enable)
    timer.control |= (1 << 12);
  }

  timer.counter = counter;
}

void core_t::timer_irq(int n) {
  auto &timer = timers[n];

  // 6     IRQ Once/Repeat Mode    (0=One-shot, 1=Repeatedly)
  // 7     IRQ Pulse/Toggle Mode   (0=Short Bit10=0 Pulse, 1=Toggle Bit10 on/off)
  // 10    Interrupt Request       (0=Yes, 1=No) (Set after Writing)    (W=1) (R)

  // Latch old bi10 value for edge detection.
  int old_bit_10 = timer.control & (1 << 10);

  // Either toggle bit10 or clear it, based on pulse/toggle mode.
  if ((timer.control & (1 << 7)) != 0) {
    timer.control ^= (1 << 10);
  } else {
    timer.control &= ~(1 << 10);
  }

  // Latch new bit10 value for edge detection.
  int new_bit_10 = timer.control & (1 << 10);

  if (timer.irq_enable && old_bit_10 > new_bit_10) {
    log("Sending interrupt for timer %d", n);

    // Interrupts are enabled, and we had a falling-edge of bit10.
    switch (n) {
      case 0: irq->interrupt(interrupt_type_t::timer0); break;
      case 1: irq->interrupt(interrupt_type_t::timer1); break;
      case 2: irq->interrupt(interrupt_type_t::timer2); break;
    }
  }

  if ((timer.control & (1 << 7)) == 0) {
    // Short bit10 pulse, so we'll just set it back.
    timer.control |= (1 << 10);
  }

  if ((timer.control & (1 << 6)) == 0) {
    // Disable further interrupts for one-shot mode.
    timer.irq_enable = false;
  }
}

uint16_t core_t::io_read_half(uint32_t address) {
  int n = (address >> 4) & 3;

  switch (address & 0x1F80110F) {
    case 0x1F801100:
      return timers[n].counter;

    case 0x1F801104:
      return timers[n].control;

    case 0x1F801108:
      return timers[n].counter_target;
  }

  return 0;
}

uint32_t core_t::io_read_word(uint32_t address) {
  return io_read_half(address);
}

void core_t::io_write_half(uint32_t address, uint16_t data) {
  int n = (address >> 4) & 3;

  switch (address & 0x1F80110F) {
    case 0x1F801100:
      timers[n].counter = data;
      return;

    case 0x1F801104:
      timers[n].control &= ~0x3ff;
      timers[n].control |= data & 0x3ff;
      timers[n].control |= 0x400;
      timers[n].irq_enable = 1;
      return;

    case 0x1F801108:
      timers[n].counter_target = data;
      return;
  }
}
