#ifndef __R3051_CORE_T_HPP__
#define __R3051_CORE_T_HPP__

#include "bus_t.hpp"
#include "cop0_t.hpp"
#include <stdint.h>

namespace r3051 {
    struct registers_t {
        uint32_t all[32];
        uint32_t lo;
        uint32_t hi;
        uint32_t pc;
        uint32_t next_pc;
    };

    class core_t {
    private:
        registers_t regs;
        bus_t &bus;
        cop0_t &cop0;

    public:
        core_t(bus_t&, cop0_t&);

        void main(void);
    };
}

#endif
