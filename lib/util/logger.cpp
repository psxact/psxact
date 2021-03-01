#include "util/logger.hpp"

using namespace psx::util;

logger::logger(const char *name, bool enabled)
		: name(name)
		, enabled(enabled) {
}

#ifdef WITH_LOGGING
void logger::log(const char *fmt, ...) {
  if (enabled) {
    printf("[%s] ", name);

    va_list arg;
    va_start(arg, format);
    vprintf(format, arg);
    va_end(arg);

    printf("\n");
  }
}
#endif
