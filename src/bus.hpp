#ifndef __r3051_bus_hpp__
#define __r3051_bus_hpp__

#include <stdint.h>

namespace gpu {
    class core_t;
}

namespace spu {
    class core_t;
}

#define BYTE 0
#define HALF 1
#define WORD 2

class bus_t {
private:
    gpu::core_t &gpu;
    spu::core_t &spu;

public:
    bus_t(gpu::core_t &, spu::core_t &);

    ~bus_t();

    uint32_t read(int, uint32_t);

    void write(int, uint32_t, uint32_t);
};

#endif
