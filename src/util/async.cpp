#include "util/async.hpp"

using namespace psx::util;

auto async::delay(int t) -> async_t {
  return async::wait(t, &async::done);
}

auto async::done() -> async_t {
  return async_t();
}

auto async::wait(int time, typename async_wait_t::K cont) -> async_t {
  async_wait_t wait;
  wait.time = time;
  wait.cont = cont;

  return async_t(wait);
}

auto async::tick(int time, async_t async) -> async_t {
  if (async.type == async_type_t::exit) {
    return async;
  }

  if (async.wait.time == time) {
    return async.wait.cont();
  }
  else {
    if (async.wait.time > time) {
      async.wait.time -= time;
      return async;
    }
    else {
      return tick(time - async.wait.time, async.wait.cont());
    }
  }
}
