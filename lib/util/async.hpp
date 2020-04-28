#ifndef UTIL_ASYNC_HPP_
#define UTIL_ASYNC_HPP_

#include <functional>

namespace psx::util {

  enum class async_type_t {
    exit,
    wait
  };

  struct async_t;

  struct async_wait_t {
    using K = std::function<async_t()>;

    int time;
    K cont;
  };

  struct async_t {
    async_type_t type;
    async_wait_t wait;

    async_t()
      : type(async_type_t::exit) {
    }

    async_t(async_wait_t wait)
      : type(async_type_t::wait)
      , wait(wait) {
    }

    ~async_t() {
    }

    async_t then(std::function<async_t()> f) {
      if (type == async_type_t::exit) {
        return f();
      }
      else {
        async_wait_t next;
        next.time = wait.time;
        next.cont = [&]() {
          return wait.cont().then(f);
        };

        return async_t(next);
      }
    }
  };

  namespace async {
    async_t delay(int t);
    async_t done();
    async_t wait(int time, typename async_wait_t::K cont);
    async_t tick(int time, async_t async);
  }
}

#endif
