#ifndef __utility_hpp__
#define __utility_hpp__

#include <stdio.h>
#include <stdint.h>

namespace utility {
    using namespace std;

    inline void read_all_bytes(const char *filename, uint8_t *result, int skip, int take) {
        FILE* file;

        fopen_s(&file, filename, "rb+");
        fseek(file, skip, SEEK_SET);
        fread_s(result, take, 1, take, file);
        fclose(file);
    }

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
