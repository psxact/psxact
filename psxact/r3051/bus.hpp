#ifndef __r3051_bus_t_hpp__
#define __r3051_bus_t_hpp__

#include <stdint.h>

namespace r3051 {
#define BYTE 0
#define HALF 1
#define WORD 2

    class bus_t {
    public:
        bus_t();

        ~bus_t();

        uint32_t read(int, uint32_t);

        void write(int, uint32_t, uint32_t);
    };
}

#endif
