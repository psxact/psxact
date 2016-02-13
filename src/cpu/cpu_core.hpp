#ifndef __cpu_core_hpp__
#define __cpu_core_hpp__

#include <stdint.h>

class bus_t;

namespace cpu {
    struct cop0_t {
        uint32_t regs[16];
    };

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

    struct segment_t {
        uint32_t mask;
        bool cached;
    };

    class core_t {
    private:
        bus_t &bus;
        cop0_t cop0;
        registers_t regs;

    public:
        core_t(bus_t&);

        void main();

        void enter_exception(uint32_t excode, uint32_t epc);

        void leave_exception();

        uint32_t read_code();

        uint32_t read_data(int, uint32_t);

        void write_data(int, uint32_t, uint32_t);
    };
}

#endif
