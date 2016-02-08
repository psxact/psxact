#ifndef __r3051_core_t_hpp__
#define __r3051_core_t_hpp__

#include "bus_t.hpp"
#include "cop0_t.hpp"
#include <stdint.h>

namespace r3051 {
    struct registers_t {
        union {
            uint32_t u[32];
            int32_t i[32];
        };

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

        uint32_t enter_exception(uint32_t excode, uint32_t epc);

        uint32_t leave_exception();
    };
}

#endif
