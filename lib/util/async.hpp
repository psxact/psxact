#ifndef UTIL_ASYNC_HPP_
#define UTIL_ASYNC_HPP_

#include <functional>

namespace psx::util {

  enum class async_type {
    exit,
    wait
  };

  struct async;

  struct async_wait {
    using K = std::function<async()>;

    int time;
    K cont;
  };

  struct async {
    async_type type;
    async_wait wait;

    async();
    async(int time, typename async_wait::K cont);

    async then(std::function<async()> f);
    async tick(int time);
  };
}

#endif
