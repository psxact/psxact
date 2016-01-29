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

inline uint32_t get_physical_address(uint32_t address) {
    if ((address & 0xe0000000) == 0x80000000) { return address & 0x1fffffff; } // kseg0
    if ((address & 0xe0000000) == 0xa0000000) { return address & 0x1fffffff; } // kseg1
    if ((address & 0xc0000000) == 0xc0000000) { return address & 0x3fffffff; } // kseg2

    return address; // kuseg
}

#define between(a, b) ((address & ~((a) ^ (b))) == (a))

uint32_t bus_t::read_code(uint32_t address) {
    address = get_physical_address(address);

    if (between(0x00000000, 0x001fffff)) { // wram
        return read_word(wram, address & 0x1ffffc);
    }

    if (between(0x1fc00000, 0x1fc7ffff)) { // bios
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

    if (between(0x00000000, 0x001fffff)) { // wram
        switch (size) {
        case BYTE: return read_byte(wram, address & 0x1fffff);
        case HALF: return read_half(wram, address & 0x1ffffe);
        case WORD: return read_word(wram, address & 0x1ffffc);
        }
    }

    if (between(0x1fc00000, 0x1fc7ffff)) { // bios
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

    if (between(0x00000000, 0x001fffff)) { // wram
        switch (size) {
        case BYTE: return write_byte(wram, address & 0x1fffff, data);
        case HALF: return write_half(wram, address & 0x1ffffe, data);
        case WORD: return write_word(wram, address & 0x1ffffc, data);
        }
    }

    if (between(0x1fc00000, 0x1fc7ffff)) { // bios
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

    case 0x1f801100: return; // timer 0 - counter value
    case 0x1f801104: return; // timer 0 - counter mode
    case 0x1f801108: return; // timer 0 - counter target

    case 0x1f801110: return; // timer 1 - counter value
    case 0x1f801114: return; // timer 1 - counter mode
    case 0x1f801118: return; // timer 1 - counter target

    case 0x1f801120: return; // timer 2 - counter value
    case 0x1f801124: return; // timer 2 - counter mode
    case 0x1f801128: return; // timer 2 - counter target

    case 0x1f801c00: return; // voice 0 volume left
    case 0x1f801c02: return; // voice 0 volume right
    case 0x1f801c04: return; // voice 0 adpcm sample rate
    case 0x1f801c06: return; // voice 0 adpcm start address
    case 0x1f801c08: return; // voice 0 attack/decay
    case 0x1f801c0a: return; // voice 0 sustain/release
    case 0x1f801c0c: return; // voice 0 adsr current volume
    case 0x1f801c0e: return; // voice 0 adpcm repeat address

    case 0x1f801c10: return; // voice 1 volume left
    case 0x1f801c12: return; // voice 1 volume right
    case 0x1f801c14: return; // voice 1 adpcm sample rate
    case 0x1f801c16: return; // voice 1 adpcm start address
    case 0x1f801c18: return; // voice 1 attack/decay
    case 0x1f801c1a: return; // voice 1 sustain/release
    case 0x1f801c1c: return; // voice 1 adsr current volume
    case 0x1f801c1e: return; // voice 1 adpcm repeat address

    case 0x1f801c20: return; // voice 2 volume left
    case 0x1f801c22: return; // voice 2 volume right
    case 0x1f801c24: return; // voice 2 adpcm sample rate
    case 0x1f801c26: return; // voice 2 adpcm start address
    case 0x1f801c28: return; // voice 2 attack/decay
    case 0x1f801c2a: return; // voice 2 sustain/release
    case 0x1f801c2c: return; // voice 2 adsr current volume
    case 0x1f801c2e: return; // voice 2 adpcm repeat address

    case 0x1f801c30: return; // voice 3 volume left
    case 0x1f801c32: return; // voice 3 volume right
    case 0x1f801c34: return; // voice 3 adpcm sample rate
    case 0x1f801c36: return; // voice 3 adpcm start address
    case 0x1f801c38: return; // voice 3 attack/decay
    case 0x1f801c3a: return; // voice 3 sustain/release
    case 0x1f801c3c: return; // voice 3 adsr current volume
    case 0x1f801c3e: return; // voice 3 adpcm repeat address

    case 0x1f801c40: return; // voice 4 volume left
    case 0x1f801c42: return; // voice 4 volume right
    case 0x1f801c44: return; // voice 4 adpcm sample rate
    case 0x1f801c46: return; // voice 4 adpcm start address
    case 0x1f801c48: return; // voice 4 attack/decay
    case 0x1f801c4a: return; // voice 4 sustain/release
    case 0x1f801c4c: return; // voice 4 adsr current volume
    case 0x1f801c4e: return; // voice 4 adpcm repeat address

    case 0x1f801c50: return; // voice 5 volume left
    case 0x1f801c52: return; // voice 5 volume right
    case 0x1f801c54: return; // voice 5 adpcm sample rate
    case 0x1f801c56: return; // voice 5 adpcm start address
    case 0x1f801c58: return; // voice 5 attack/decay
    case 0x1f801c5a: return; // voice 5 sustain/release
    case 0x1f801c5c: return; // voice 5 adsr current volume
    case 0x1f801c5e: return; // voice 5 adpcm repeat address

    case 0x1f801c60: return; // voice 6 volume left
    case 0x1f801c62: return; // voice 6 volume right
    case 0x1f801c64: return; // voice 6 adpcm sample rate
    case 0x1f801c66: return; // voice 6 adpcm start address
    case 0x1f801c68: return; // voice 6 attack/decay
    case 0x1f801c6a: return; // voice 6 sustain/release
    case 0x1f801c6c: return; // voice 6 adsr current volume
    case 0x1f801c6e: return; // voice 6 adpcm repeat address

    case 0x1f801c70: return; // voice 7 volume left
    case 0x1f801c72: return; // voice 7 volume right
    case 0x1f801c74: return; // voice 7 adpcm sample rate
    case 0x1f801c76: return; // voice 7 adpcm start address
    case 0x1f801c78: return; // voice 7 attack/decay
    case 0x1f801c7a: return; // voice 7 sustain/release
    case 0x1f801c7c: return; // voice 7 adsr current volume
    case 0x1f801c7e: return; // voice 7 adpcm repeat address

    case 0x1f801c80: return; // voice 8 volume left
    case 0x1f801c82: return; // voice 8 volume right
    case 0x1f801c84: return; // voice 8 adpcm sample rate
    case 0x1f801c86: return; // voice 8 adpcm start address
    case 0x1f801c88: return; // voice 8 attack/decay
    case 0x1f801c8a: return; // voice 8 sustain/release
    case 0x1f801c8c: return; // voice 8 adsr current volume
    case 0x1f801c8e: return; // voice 8 adpcm repeat address

    case 0x1f801c90: return; // voice 9 volume left
    case 0x1f801c92: return; // voice 9 volume right
    case 0x1f801c94: return; // voice 9 adpcm sample rate
    case 0x1f801c96: return; // voice 9 adpcm start address
    case 0x1f801c98: return; // voice 9 attack/decay
    case 0x1f801c9a: return; // voice 9 sustain/release
    case 0x1f801c9c: return; // voice 9 adsr current volume
    case 0x1f801c9e: return; // voice 9 adpcm repeat address

    case 0x1f801ca0: return; // voice 10 volume left
    case 0x1f801ca2: return; // voice 10 volume right
    case 0x1f801ca4: return; // voice 10 adpcm sample rate
    case 0x1f801ca6: return; // voice 10 adpcm start address
    case 0x1f801ca8: return; // voice 10 attack/decay
    case 0x1f801caa: return; // voice 10 sustain/release
    case 0x1f801cac: return; // voice 10 adsr current volume
    case 0x1f801cae: return; // voice 10 adpcm repeat address

    case 0x1f801cb0: return; // voice 11 volume left
    case 0x1f801cb2: return; // voice 11 volume right
    case 0x1f801cb4: return; // voice 11 adpcm sample rate
    case 0x1f801cb6: return; // voice 11 adpcm start address
    case 0x1f801cb8: return; // voice 11 attack/decay
    case 0x1f801cba: return; // voice 11 sustain/release
    case 0x1f801cbc: return; // voice 11 adsr current volume
    case 0x1f801cbe: return; // voice 11 adpcm repeat address

    case 0x1f801cc0: return; // voice 12 volume left
    case 0x1f801cc2: return; // voice 12 volume right
    case 0x1f801cc4: return; // voice 12 adpcm sample rate
    case 0x1f801cc6: return; // voice 12 adpcm start address
    case 0x1f801cc8: return; // voice 12 attack/decay
    case 0x1f801cca: return; // voice 12 sustain/release
    case 0x1f801ccc: return; // voice 12 adsr current volume
    case 0x1f801cce: return; // voice 12 adpcm repeat address

    case 0x1f801cd0: return; // voice 13 volume left
    case 0x1f801cd2: return; // voice 13 volume right
    case 0x1f801cd4: return; // voice 13 adpcm sample rate
    case 0x1f801cd6: return; // voice 13 adpcm start address
    case 0x1f801cd8: return; // voice 13 attack/decay
    case 0x1f801cda: return; // voice 13 sustain/release
    case 0x1f801cdc: return; // voice 13 adsr current volume
    case 0x1f801cde: return; // voice 13 adpcm repeat address

    case 0x1f801ce0: return; // voice 14 volume left
    case 0x1f801ce2: return; // voice 14 volume right
    case 0x1f801ce4: return; // voice 14 adpcm sample rate
    case 0x1f801ce6: return; // voice 14 adpcm start address
    case 0x1f801ce8: return; // voice 14 attack/decay
    case 0x1f801cea: return; // voice 14 sustain/release
    case 0x1f801cec: return; // voice 14 adsr current volume
    case 0x1f801cee: return; // voice 14 adpcm repeat address

    case 0x1f801cf0: return; // voice 15 volume left
    case 0x1f801cf2: return; // voice 15 volume right
    case 0x1f801cf4: return; // voice 15 adpcm sample rate
    case 0x1f801cf6: return; // voice 15 adpcm start address
    case 0x1f801cf8: return; // voice 15 attack/decay
    case 0x1f801cfa: return; // voice 15 sustain/release
    case 0x1f801cfc: return; // voice 15 adsr current volume
    case 0x1f801cfe: return; // voice 15 adpcm repeat address

    case 0x1f801d00: return; // voice 16 volume left
    case 0x1f801d02: return; // voice 16 volume right
    case 0x1f801d04: return; // voice 16 adpcm sample rate
    case 0x1f801d06: return; // voice 16 adpcm start address
    case 0x1f801d08: return; // voice 16 attack/decay
    case 0x1f801d0a: return; // voice 16 sustain/release
    case 0x1f801d0c: return; // voice 16 adsr current volume
    case 0x1f801d0e: return; // voice 16 adpcm repeat address

    case 0x1f801d10: return; // voice 17 volume left
    case 0x1f801d12: return; // voice 17 volume right
    case 0x1f801d14: return; // voice 17 adpcm sample rate
    case 0x1f801d16: return; // voice 17 adpcm start address
    case 0x1f801d18: return; // voice 17 attack/decay
    case 0x1f801d1a: return; // voice 17 sustain/release
    case 0x1f801d1c: return; // voice 17 adsr current volume
    case 0x1f801d1e: return; // voice 17 adpcm repeat address

    case 0x1f801d20: return; // voice 18 volume left
    case 0x1f801d22: return; // voice 18 volume right
    case 0x1f801d24: return; // voice 18 adpcm sample rate
    case 0x1f801d26: return; // voice 18 adpcm start address
    case 0x1f801d28: return; // voice 18 attack/decay
    case 0x1f801d2a: return; // voice 18 sustain/release
    case 0x1f801d2c: return; // voice 18 adsr current volume
    case 0x1f801d2e: return; // voice 18 adpcm repeat address

    case 0x1f801d30: return; // voice 19 volume left
    case 0x1f801d32: return; // voice 19 volume right
    case 0x1f801d34: return; // voice 19 adpcm sample rate
    case 0x1f801d36: return; // voice 19 adpcm start address
    case 0x1f801d38: return; // voice 19 attack/decay
    case 0x1f801d3a: return; // voice 19 sustain/release
    case 0x1f801d3c: return; // voice 19 adsr current volume
    case 0x1f801d3e: return; // voice 19 adpcm repeat address

    case 0x1f801d40: return; // voice 20 volume left
    case 0x1f801d42: return; // voice 20 volume right
    case 0x1f801d44: return; // voice 20 adpcm sample rate
    case 0x1f801d46: return; // voice 20 adpcm start address
    case 0x1f801d48: return; // voice 20 attack/decay
    case 0x1f801d4a: return; // voice 20 sustain/release
    case 0x1f801d4c: return; // voice 20 adsr current volume
    case 0x1f801d4e: return; // voice 20 adpcm repeat address

    case 0x1f801d50: return; // voice 21 volume left
    case 0x1f801d52: return; // voice 21 volume right
    case 0x1f801d54: return; // voice 21 adpcm sample rate
    case 0x1f801d56: return; // voice 21 adpcm start address
    case 0x1f801d58: return; // voice 21 attack/decay
    case 0x1f801d5a: return; // voice 21 sustain/release
    case 0x1f801d5c: return; // voice 21 adsr current volume
    case 0x1f801d5e: return; // voice 21 adpcm repeat address

    case 0x1f801d60: return; // voice 22 volume left
    case 0x1f801d62: return; // voice 22 volume right
    case 0x1f801d64: return; // voice 22 adpcm sample rate
    case 0x1f801d66: return; // voice 22 adpcm start address
    case 0x1f801d68: return; // voice 22 attack/decay
    case 0x1f801d6a: return; // voice 22 sustain/release
    case 0x1f801d6c: return; // voice 22 adsr current volume
    case 0x1f801d6e: return; // voice 22 adpcm repeat address

    case 0x1f801d70: return; // voice 23 volume left
    case 0x1f801d72: return; // voice 23 volume right
    case 0x1f801d74: return; // voice 23 adpcm sample rate
    case 0x1f801d76: return; // voice 23 adpcm start address
    case 0x1f801d78: return; // voice 23 attack/decay
    case 0x1f801d7a: return; // voice 23 sustain/release
    case 0x1f801d7c: return; // voice 23 adsr current volume
    case 0x1f801d7e: return; // voice 23 adpcm repeat address

    case 0x1f801d80: return; // main volume left
    case 0x1f801d82: return; // main volume right
    case 0x1f801d84: return; // reverb output volume left
    case 0x1f801d86: return; // reverb output volume right
    case 0x1f801d88: return; // voice 0..15 key on
    case 0x1f801d8a: return; // voice 16..23 key on
    case 0x1f801d8c: return; // voice 0..15 key off
    case 0x1f801d8e: return; // voice 16..23 key off
    case 0x1f801d90: return; // voice 0..15 pitch modulation enable flags
    case 0x1f801d92: return; // voice 16..23 pitch modulation enable flags
    case 0x1f801d94: return; // voice 0..15 noise mode enable
    case 0x1f801d96: return; // voice 16..23 noise mode enable
    case 0x1f801d98: return; // voice 0..15 reverb mode aka echo on
    case 0x1f801d9a: return; // voice 16..23 reverb mode aka echo on

    case 0x1f801da6: return; // sound ram data transfer address
    case 0x1f801da8: return; // sound ram data transfer fifo

    case 0x1f801dac: return; // sound ram data transfer control

    case 0x1f801db0: return; // cd audio left input volume
    case 0x1f801db2: return; // cd audio right input volume
    case 0x1f801db4: return; // external audio left input volume
    case 0x1f801db6: return; // external audio right input volume

    case 0x1f801daa: return; // spu control

    case 0x1f802041: return; // post
    }

    throw std::exception("unknown write");
}
