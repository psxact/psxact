#ifndef __CPU_DECODER_HPP__
#define __CPU_DECODER_HPP__

#include <stdint.h>

namespace cpu {
namespace decoder {
  uint32_t code;

  inline uint32_t uconst() {
    return ((code & 0xffff) ^ 0x0000) - 0x0000;
  }

  inline uint32_t iconst() {
    return ((code & 0xffff) ^ 0x8000) - 0x8000;
  }

  inline uint32_t op_lo() {
    return (code >>  0) & 63;
  }

  inline uint32_t sa() {
    return (code >>  6) & 31;
  }

  inline uint32_t rd() {
    return (code >> 11) & 31;
  }

  inline uint32_t rt() {
    return (code >> 16) & 31;
  }

  inline uint32_t rs() {
    return (code >> 21) & 31;
  }

  inline uint32_t op_hi() {
    return (code >> 26) & 63;
  }
}
}

#endif
