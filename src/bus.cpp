#include "bus.hpp"
#include "gpu/gpu_core.hpp"
#include "spu/spu_core.hpp"
#include "utility.hpp"
#include <stdexcept>
#include <cstring>

uint8_t *bios = new uint8_t[utility::kib<512>()];
uint8_t *wram = new uint8_t[utility::mib<  2>()];

bus_t::bus_t(gpu::core_t &gpu, spu::core_t &spu)
    : gpu(gpu)
    , spu(spu) {
    memset(bios, 0, utility::kib<512>());
    memset(wram, 0, utility::mib<  2>());

    utility::read_all_bytes("bios.rom", bios, 0, utility::kib<512>());
}

bus_t::~bus_t() {
    delete bios;
    delete wram;
}

uint32_t bus_t::read(int size, uint32_t address) {
    if (utility::between<0x00000000, 0x007fffff>(address)) { // wram
        switch (size) {
        case BYTE: return utility::read_byte(wram, address & 0x1fffff);
        case HALF: return utility::read_half(wram, address & 0x1ffffe);
        case WORD: return utility::read_word(wram, address & 0x1ffffc);
        }
    }

    if (utility::between<0x1f000000, 0x1f7fffff>(address)) {
        printf("expansion region 1 read: $%08x\n", address);
        return 0;
    }

    if (utility::between<0x1f800000, 0x1f8003ff>(address)) {
        printf("scratchpad read: $%08x\n", address);
        return 0;
    }

    if (utility::between<0x1f801000, 0x1f801fff>(address)) {
        printf("mmio read: $%08x\n", address);
        return 0;
    }

    if (utility::between<0x1f802000, 0x1f802fff>(address)) {
        printf("expansion region 2 read: $%08x\n", address);
        return 0;
    }

    if (utility::between<0x1fa00000, 0x1fbfffff>(address)) {
        printf("expansion region 3 read: $%08x\n", address);
        return 0;
    }

    if (utility::between<0x1fc00000, 0x1fc7ffff>(address)) {
        switch (size) {
        case BYTE: return utility::read_byte(bios, address & 0x7ffff);
        case HALF: return utility::read_half(bios, address & 0x7fffe);
        case WORD: return utility::read_word(bios, address & 0x7fffc);
        }
    }

    throw std::runtime_error("unknown read");
}

void bus_t::write(int size, uint32_t address, uint32_t data) {
    if (utility::between<0x00000000, 0x007fffff>(address)) {
        switch (size) {
        case BYTE: return utility::write_byte(wram, address & 0x1fffff, data);
        case HALF: return utility::write_half(wram, address & 0x1ffffe, data);
        case WORD: return utility::write_word(wram, address & 0x1ffffc, data);
        }
    }

    if (utility::between<0x1f000000, 0x1f7fffff>(address)) {
        printf("expansion region 1 write: $%08x <- $%08x\n", address, data);
        return;
    }

    if (utility::between<0x1f800000, 0x1f8003ff>(address)) {
        printf("scratchpad write: $%08x <- $%08x\n", address, data);
        return;
    }

    if (utility::between<0x1f801000, 0x1f801fff>(address)) {
        printf("mmio write: $%08x <- $%08x\n", address, data);
        return;
    }

    if (utility::between<0x1f802000, 0x1f802fff>(address)) {
        printf("expansion region 2 write: $%08x <- $%08x\n", address, data);
        return;
    }

    if (utility::between<0x1fa00000, 0x1fbfffff>(address)) {
        printf("expansion region 3 write: $%08x <- $%08x\n", address, data);
        return;
    }

    if (utility::between<0x1fc00000, 0x1fc7ffff>(address)) {
        printf("bios write: $%08x <- $%08x\n", address, data);
        return;
    }

    throw std::runtime_error("unknown write");
}
