#include "util/panic.hpp"

#include <cstdarg>
#include <cstdlib>
#include <cstdio>

void psx::util::panic(const char *format, ...) {
  printf("Panic! ");

  va_list arg;
  va_start(arg, format);
  vprintf(format, arg);
  va_end(arg);

  printf("\n");

  exit(EXIT_FAILURE);
}
