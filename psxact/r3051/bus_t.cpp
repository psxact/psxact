#include "bus_t.hpp"
#include "cop0_t.hpp"
#include "../utility.hpp"

using namespace r3051;

char* bios = new char[512 << 10];
char* wram = new char[  2 << 20];

static inline uint32_t read_byte(char* buffer, uint32_t address) {
    return buffer[address] & 0xff;
}

static inline uint32_t read_half(char* buffer, uint32_t address) {
    return
        (read_byte(buffer, address &~1) << 0) |
        (read_byte(buffer, address | 1) << 8);
}

static inline uint32_t read_word(char* buffer, uint32_t address) {
    return
        (read_half(buffer, address &~2) << 0) |
        (read_half(buffer, address | 2) << 16);
}

static inline void write_byte(char* buffer, uint32_t address, uint32_t data) {
    buffer[address] = data & 0xff;
}

static inline void write_half(char* buffer, uint32_t address, uint32_t data) {
    write_byte(buffer, address &~1, data);
    write_byte(buffer, address | 1, data >> 8);
}

static inline void write_word(char* buffer, uint32_t address, uint32_t data) {
    write_half(buffer, address &~2, data);
    write_half(buffer, address | 2, data >> 16);
}

bus_t::bus_t(cop0_t &cop0)
    : cop0(cop0) {

    if (utility::read_all_bytes("bios.rom", bios) != 524288) {
        throw std::exception("bios must be 512KiB");
    }
}

static inline uint32_t get_physical_address(uint32_t address) {
    if ((address & 0xe0000000) == 0x80000000) { return address & 0x1fffffff; } // kseg0
    if ((address & 0xe0000000) == 0xa0000000) { return address & 0x1fffffff; } // kseg1
    if ((address & 0xc0000000) == 0xc0000000) { return address & 0x3fffffff; } // kseg2

    return address; // kuseg
}

uint32_t bus_t::read_code(uint32_t address) {
    address = get_physical_address(address);

    if (utility::between<0x00000000, 0x001fffff>(address)) { // wram
        return read_word(wram, address & 0x1ffffc);
    }

    if (utility::between<0x1fc00000, 0x1fc7ffff>(address)) { // bios
        return read_word(bios, address & 0x7fffc);
    }

    return 0;
}

uint32_t bus_t::read_data(bus_size_t size, uint32_t address) {
    if (size == WORD) {
        switch ((cop0.get_register(12) >> 16) & 3) {
        case 0: break;    // swc=0, isc=0
        case 1: return 0; // swc=0, isc=1 (read from d-cache)
        case 2: break;    // swc=1, isc=0
        case 3: return 0; // swc=1, isc=1 (read from i-cache)
        }
    }

    address = get_physical_address(address);

    if (utility::between<0x00000000, 0x007fffff>(address)) { // wram
        switch (size) {
        case BYTE: return read_byte(wram, address & 0x1fffff);
        case HALF: return read_half(wram, address & 0x1ffffe);
        case WORD: return read_word(wram, address & 0x1ffffc);
        }
    }

    if (utility::between<0x1fc00000, 0x1fc7ffff>(address)) { // bios
        switch (size) {
        case BYTE: return read_byte(bios, address & 0x7ffff);
        case HALF: return read_half(bios, address & 0x7fffe);
        case WORD: return read_word(bios, address & 0x7fffc);
        }
    }

    switch (address) {
    case 0x1f000084: return 0; // pre-boot id

    case 0x1f801070: return 0; // i_stat
    case 0x1f801074: return 0; // i_mask

    case 0x1f8010f0: return 0; // dma control

    case 0x1f801d88: return 0; // voice 0..15 key on
    case 0x1f801d8a: return 0; // voice 16..23 key on
    case 0x1f801d8c: return 0; // voice 0..15 key off
    case 0x1f801d8e: return 0; // voice 16..23 key off

    case 0x1f801daa: return 0; // spu control
    case 0x1f801dac: return 0; // sound ram data transfer control

    case 0x1f801dae: return 0; // spu status
    }

    throw std::exception("unknown read");
}

void bus_t::write_data(bus_size_t size, uint32_t address, uint32_t data) {
    if (size == WORD) {
        switch ((cop0.get_register(12) >> 16) & 3) {
        case 0: break;  // swc=0, isc=0
        case 1: return; // swc=0, isc=1 (write to d-cache)
        case 2: break;  // swc=1, isc=0
        case 3: return; // swc=1, isc=1 (write to i-cache)
        }
    }

    if (address == 0xfffe0130) {
        return; // cache control
    }

    address = get_physical_address(address);

    if (utility::between<0x00000000, 0x007fffff>(address)) { // wram
        switch (size) {
        case BYTE: return write_byte(wram, address & 0x1fffff, data);
        case HALF: return write_half(wram, address & 0x1ffffe, data);
        case WORD: return write_word(wram, address & 0x1ffffc, data);
        }
    }

    if (utility::between<0x1fc00000, 0x1fc7ffff>(address)) { // bios
        return;
    }

    if (utility::between<0x1f801080, 0x1f8010ff>(address)) { // dma
        return;
    }

    if (utility::between<0x1f801100, 0x1f80113f>(address)) { // timers
        return;
    }

    if (utility::between<0x1f801800, 0x1f801803>(address)) { // cd-rom
        return;
    }

    if (utility::between<0x1f801810, 0x1f801817>(address)) { // gpu
        return;
    }

    if (utility::between<0x1f801c00, 0x1f801fff>(address)) { // spu
        return;
    }

    switch (address) {
    case 0x1f801000: return; // expansion 1 base address
    case 0x1f801004: return; // expansion 2 base address
    case 0x1f801008: return; // expansion 1 delay/size
    case 0x1f80100c: return; // expansion 3 delay/size
    case 0x1f801010: return; // bios_rom delay/size
    case 0x1f801014: return; // spu_delay delay/size
    case 0x1f801018: return; // cdrom_delay delay/size
    case 0x1f80101c: return; // expansion 2 delay/size
    case 0x1f801020: return; // common delay

    case 0x1f801060: return; // ram_size

    case 0x1f801070: return; // i_stat
    case 0x1f801074: return; // i_mask

    case 0x1f8010f0: return; // dma control

    case 0x1f802041: return; // post
    }

    throw std::exception("unknown write");
}
