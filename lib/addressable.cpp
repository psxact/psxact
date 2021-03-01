#include "addressable.hpp"

#include "util/unused.hpp"
#include "util/panic.hpp"

using namespace psx;
using namespace psx::util;

addressable::addressable(opts &o, component c)
  : logger(o.get_log_name(c), o.get_log_enabled(c)) {
}

uint32_t addressable::io_read(address_width width, uint32_t address) {
	MAYBE_UNUSED(width);
	MAYBE_UNUSED(address);

	PANIC("Unhandled I/O read. io_read(%d, 0x%08x)", int(width), address);

  return 0;
}

void addressable::io_write(address_width width, uint32_t address, uint32_t data) {
	MAYBE_UNUSED(width);
	MAYBE_UNUSED(address);
	MAYBE_UNUSED(data);

  PANIC("Unhandled I/O write. io_write(%d, 0x%08x, 0x%08x)", int(width), address, data);
}
