#include "util/async.hpp"

using namespace psx::util;

async_t::async_t()
  : type(async_type_t::exit)
  , wait() {
}

async_t::async_t(int time, typename async_wait_t::K cont)
  : type(async_type_t::wait)
  , wait({ time, cont }) {
}

auto async_t::then(std::function<async_t()> f) -> async_t {
  if (type == async_type_t::exit) {
    return f();
  }

  return async_t(wait.time, [&]() {
    return wait.cont().then(f);
  });
}

auto async_t::tick(int time) -> async_t {
  if (type == async_type_t::exit) {
    return *this;
  }

  if (wait.time > time) {
    wait.time -= time;
    return *this;
  } else {
    return wait.cont().tick(time - wait.time);
  }
}
