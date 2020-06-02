#ifndef UTIL_PANIC_HPP_
#define UTIL_PANIC_HPP_

#include "util/base.hpp"

namespace psx::util {

  void panic(const char *format, ...)
      attr_noreturn
      attr_printf(1, 2);
}

#endif // UTIL_PANIC_HPP_
