// Copyright 2018 psxact

#ifndef EXP_EXPANSION2_HPP_
#define EXP_EXPANSION2_HPP_

#include "memory-component.hpp"

namespace psx {
namespace exp {

class expansion2_t : public memory_component_t {
 public:
  expansion2_t();

  void io_write_byte(uint32_t address, uint8_t data);
};

}  // namespace exp
}  // namespace psx

#endif  // EXP_EXPANSION2_HPP_
