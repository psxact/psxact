// Copyright 2018 psxact

#ifndef MDEC_CORE_HPP_
#define MDEC_CORE_HPP_

#include "console.hpp"
#include "memory-component.hpp"

namespace psx {
namespace mdec {

class core_t : public memory_component_t {
 public:
  core_t();
};

}  // namespace mdec
}  // namespace psx

#endif  // MDEC_CORE_HPP_
