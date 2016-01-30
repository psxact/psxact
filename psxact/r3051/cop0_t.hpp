#ifndef __r3051_cop0_t_hpp__
#define __r3051_cop0_t_hpp__

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
