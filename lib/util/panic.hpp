#ifndef UTIL_PANIC_HPP_
#define UTIL_PANIC_HPP_

#include "util/base.hpp"

#ifdef WITH_PANIC
namespace psx::util {

  void panic(const char *format, ...)
      attr_printf(1, 2);

  void panic_if(bool condition, const char *format, ...)
      attr_printf(2, 3);
}

#define PANIC(fmt, ...) psx::util::panic(fmt, ## __VA_ARGS__)
#define PANIC_IF(c, fmt, ...) psx::util::panic_if(c, fmt, ## __VA_ARGS__)
#else
#define PANIC(fmt, ...)
#define PANIC_IF(c, fmt, ...)
#endif

#endif // UTIL_PANIC_HPP_
