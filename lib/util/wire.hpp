#ifndef UTIL_WIRE_HPP_
#define UTIL_WIRE_HPP_

#include <functional>
#include <vector>
#include "interruptible.hpp"

namespace psx::util {

  enum class wire_state {
    off,
    on
  };

  /// Models a connection between two components
  class wire {
    std::vector<std::function<void()>> on_receivers;
    std::vector<std::function<void()>> off_receivers;
    std::vector<std::function<void()>> rise_receivers;
    std::vector<std::function<void()>> fall_receivers;
    wire_state state;

  public:
    virtual void recv_on(std::function<void()> receiver);
    virtual void recv_off(std::function<void()> receiver);
    virtual void recv_rise(std::function<void()> receiver);
    virtual void recv_fall(std::function<void()> receiver);

    virtual void operator()(wire_state val);
  };
}

#endif
