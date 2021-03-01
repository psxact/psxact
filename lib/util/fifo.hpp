#ifndef UTIL_FIFO_HPP_
#define UTIL_FIFO_HPP_

#include <cstdint>

#include "util/panic.hpp"

namespace psx::util {

  template<typename T, int bits>
  class fifo {
    static const int32_t mask     = (1 << (bits + 1)) - 1;
    static const int32_t mask_lsb = (1 << bits) - 1;
    static const int32_t mask_msb = (1 << bits);
    static const int32_t kSize    = (1 << bits);

    T buffer[kSize];
    uint32_t rd_ptr;
    uint32_t wr_ptr;

  public:
    void clear() {
      rd_ptr = 0;
      wr_ptr = 0;
    }

    void discard(int how_many) {
      PANIC_IF(how_many >= size(), "attempt to discard %d/%d items", how_many, size());
      rd_ptr = (rd_ptr + how_many) & mask;
    }

    const T &at(int index) const {
      PANIC_IF(index >= size(), "attempt to read index %d/%d", index, size());
      return buffer[(rd_ptr + index) & mask_lsb];
    }

    const T &read() {
      PANIC_IF(is_empty(), "attempt to read from empty fifo");
      T &value = buffer[rd_ptr & mask_lsb];
      rd_ptr = (rd_ptr + 1) & mask;

      return value;
    }

    void write(const T &value) {
      PANIC_IF(is_full(), "attempt to write to full fifo");
      buffer[wr_ptr & mask_lsb] = value;
      wr_ptr = (wr_ptr + 1) & mask;
    }

    bool is_empty() const {
      return rd_ptr == wr_ptr;
    }

    bool is_full() const {
      return rd_ptr == (wr_ptr ^ mask_msb);
    }

    int size() const {
      return (wr_ptr - rd_ptr) & mask;
    }
  };
}

#endif
