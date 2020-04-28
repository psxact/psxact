#ifndef IRQ_LINE_HPP_
#define IRQ_LINE_HPP_

#include "interruptible.hpp"

namespace psx {

  enum class irq_line_state_t {
    clear,
    active
  };

  /// Models an IRQ line connected to the CPU.
  struct irq_line_t {
    interruptible_t &backend;
    interrupt_type_t type;
    irq_line_state_t state;

    irq_line_t(interruptible_t &backend, interrupt_type_t type);

    /// Puts the IRQ line into a specified state.
    void operator()(irq_line_state_t val);
    void operator=(const psx::irq_line_t &other);
  };
}

#endif  // IRQ_LINE_HPP_
