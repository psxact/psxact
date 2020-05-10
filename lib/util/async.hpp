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

    async_t();
    async_t(int time, typename async_wait_t::K cont);

    async_t then(std::function<async_t()> f);
    async_t tick(int time);
  };
}

#endif
