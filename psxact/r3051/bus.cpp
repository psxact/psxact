#include "bus.hpp"
#include "../utility.hpp"
#include <exception>
#include <string>

using namespace r3051;

uint8_t *bios = new uint8_t[utility::kib<512>()];
uint8_t *wram = new uint8_t[utility::mib<  2>()];

static inline uint32_t read_byte(uint8_t* buffer, uint32_t address) {
    return buffer[address];
}

static inline uint32_t read_half(uint8_t* buffer, uint32_t address) {
    return
        (read_byte(buffer, address &~1) << 0) |
        (read_byte(buffer, address | 1) << 8);
}

static inline uint32_t read_word(uint8_t* buffer, uint32_t address) {
    return
        (read_half(buffer, address &~2) << 0) |
        (read_half(buffer, address | 2) << 16);
}

static inline void write_byte(uint8_t* buffer, uint32_t address, uint32_t data) {
    buffer[address] = data & 0xff;
}

static inline void write_half(uint8_t* buffer, uint32_t address, uint32_t data) {
    write_byte(buffer, address &~1, data);
    write_byte(buffer, address | 1, data >> 8);
}

static inline void write_word(uint8_t* buffer, uint32_t address, uint32_t data) {
    write_half(buffer, address &~2, data);
    write_half(buffer, address | 2, data >> 16);
}

bus_t::bus_t() {
    memset(bios, 0, utility::kib<512>());
    memset(wram, 0, utility::mib<  2>());

    utility::read_all_bytes("bios.rom", bios + 0x00000000, 0x000, utility::kib<512>());
}

bus_t::~bus_t() {
    delete bios;
    delete wram;
}

uint32_t bus_t::read(int size, uint32_t address) {
    if (utility::between<0x00000000, 0x007fffff>(address)) { // wram
        switch (size) {
        case BYTE: return read_byte(wram, address & 0x1fffff);
        case HALF: return read_half(wram, address & 0x1ffffe);
        case WORD: return read_word(wram, address & 0x1ffffc);
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
        case BYTE: return read_byte(bios, address & 0x7ffff);
        case HALF: return read_half(bios, address & 0x7fffe);
        case WORD: return read_word(bios, address & 0x7fffc);
        }
    }

    throw std::exception("unknown read");
}

void bus_t::write(int size, uint32_t address, uint32_t data) {
    if (utility::between<0x00000000, 0x007fffff>(address)) {
        switch (size) {
        case BYTE: return write_byte(wram, address & 0x1fffff, data);
        case HALF: return write_half(wram, address & 0x1ffffe, data);
        case WORD: return write_word(wram, address & 0x1ffffc, data);
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

    throw std::exception("unknown write");
}
