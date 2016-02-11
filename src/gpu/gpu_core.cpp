#include "gpu_core.hpp"
#include <exception>
#include <stdio.h>

using namespace gpu;

#define REG_BEGIN 0x1f801810

core_t::core_t() {
}

core_t::~core_t() {
}

uint32_t core_t::read(uint32_t address) {
    switch (address - REG_BEGIN) {
    case 0: return read_resp();
    case 4: return read_stat();
    }

    throw std::exception("invalid gpu read");
}

void core_t::write(uint32_t address, uint32_t data) {
    switch (address - REG_BEGIN) {
    case 0: return write_gp0(data);
    case 4: return write_gp1(data);
    }

    throw std::exception("invalid gpu write");
}

uint32_t core_t::read_resp() {
    printf("gpu::core_t::read_resp()\n");
    return 0;
}

uint32_t core_t::read_stat() {
    printf("gpu::core_t::read_stat()\n");
    return 0;
}

void core_t::write_gp0(uint32_t command) {
    printf("gpu::core_t::write_gp0(0x%08x)\n", command);
}

void core_t::write_gp1(uint32_t command) {
    printf("gpu::core_t::write_gp1(0x%08x)\n", command);
}
