#ifndef __utility_hpp__
#define __utility_hpp__

#include <stdio.h>
#include <stdint.h>

namespace utility {
    using namespace std;

    void read_all_bytes(const char *, uint8_t *, int, int);

    void debug(const char *, ...);

    uint32_t read_byte(uint8_t *, uint32_t);

    uint32_t read_half(uint8_t *, uint32_t);

    uint32_t read_word(uint8_t *, uint32_t);

    void write_byte(uint8_t *, uint32_t, uint32_t);

    void write_half(uint8_t *, uint32_t, uint32_t);

    void write_word(uint8_t *, uint32_t, uint32_t);

    template<unsigned min, unsigned max>
    inline bool between(unsigned value) {
        return (value & ~(min ^ max)) == min;
    }

    template<unsigned value>
    constexpr unsigned kib() {
        return 1024 * value;
    }

    template<unsigned value>
    constexpr unsigned mib() {
        return 1024 * kib<value>();
    }

    template<unsigned value>
    constexpr unsigned gib() {
        return 1024 * mib<value>();
    }
}

#endif
