#ifndef __gpu_core_t_hpp__
#define __gpu_core_t_hpp__

#include <stdint.h>

namespace gpu {
    class core_t {
    public:
        core_t();

        ~core_t();

        uint32_t read(uint32_t);

        void write(uint32_t, uint32_t);

    private:
        uint32_t read_resp();

        uint32_t read_stat();

        void write_gp0(uint32_t);

        void write_gp1(uint32_t);
    };
}

#endif
