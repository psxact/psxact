#include "util/async.hpp"

using namespace psx::util;

async::async()
  : type(async_type::exit)
  , wait() {
}

async::async(int time, typename async_wait::K cont)
  : type(async_type::wait)
  , wait({ time, cont }) {
}

auto async::then(std::function<async()> f) -> async {
  if (type == async_type::exit) {
    return f();
  }

  return async(wait.time, [&]() {
    return wait.cont().then(f);
  });
}

auto async::tick(int time) -> async {
  if (type == async_type::exit) {
    return *this;
  }

  if (wait.time > time) {
    wait.time -= time;
    return *this;
  } else {
    return wait.cont().tick(time - wait.time);
  }
}
