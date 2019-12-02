// Copyright 2018 psxact

#ifndef EXP_EXPANSION1_HPP_
#define EXP_EXPANSION1_HPP_

#include "memory-component.hpp"

namespace psx::exp {

class expansion1_t : public memory_component_t {
 public:
  expansion1_t();

  uint8_t io_read_byte(uint32_t address);
};

}  // namespace psx::exp

#endif  // EXP_EXPANSION1_HPP_
