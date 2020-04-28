#include "irq-line.hpp"

#include <cstdio>

using namespace psx;

irq_line_t::irq_line_t(interruptible_t &backend, interrupt_type_t type)
  : backend(backend)
  , type(type) {
}

void irq_line_t::operator()(irq_line_state_t s) {
  if (state == irq_line_state_t::clear && s == irq_line_state_t::active) {
    backend.interrupt(type);
  }

  state = s;
}

void irq_line_t::operator=(const psx::irq_line_t &other) {
  backend = other.backend;
  type = other.type;
  state = other.state;
}
