#ifndef __R3051_COP0_T_HPP__
#define __R3051_COP0_T_HPP__

#include <stdint.h>

namespace r3051 {
    class cop0_t {
    private:
        uint32_t registers[16];

    public:
        cop0_t(void);

        uint32_t get_register(uint32_t index);

        void set_register(uint32_t index, uint32_t data);
    };
}

#endif
