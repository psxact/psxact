#include "util/panic.hpp"

#include <cstdarg>
#include <cstdlib>
#include <cstdio>

#ifdef WITH_PANIC
static void print_panic(const char *format, va_list list) attr_noreturn;
static void print_panic(const char *format, va_list list) {
  printf("Panic! ");

  vprintf(format, list);

  printf("\n");

  exit(EXIT_FAILURE);
}

void psx::util::panic(const char *format, ...) {
  va_list list;
  va_start(list, format);
  print_panic(format, list);
  va_end(list);
}

void psx::util::panic_if(bool condition, const char *format, ...) {
	if (condition) {
		va_list list;
		va_start(list, format);
		print_panic(format, list);
		va_end(list);
	}
}
#endif
