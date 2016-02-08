#include "core.hpp"
#include "bus.hpp"

using namespace r3051;

static segment_t segments[8] = {
    { 0x7fffffff, true  }, // kuseg ($0000_0000 - $7fff_ffff)
    { 0x7fffffff, true  }, // 
    { 0x7fffffff, true  }, // 
    { 0x7fffffff, true  }, // 
    { 0x1fffffff, false }, // kseg0 ($8000_0000 - $9fff_ffff)
    { 0x1fffffff, false }, // kseg1 ($a000_0000 - $bfff_ffff)
    { 0x3fffffff, false }, // kseg2 ($c000_0000 - $ffff_ffff)
    { 0x3fffffff, false }  // 
};

static inline segment_t *get_segment(uint32_t address) {
    return &segments[address >> 29];
}

uint32_t core_t::read_code() {
    auto segment = get_segment(regs.pc);
    auto address = regs.pc & segment->mask;
    regs.pc = regs.next_pc;
    regs.next_pc += 4;

    return bus.read(WORD, address);
}

uint32_t core_t::read_data(int size, uint32_t address) {
    if (cop0.regs[12] & (1 << 16)) {
        return 0; // isc=1
    }

    auto segment = get_segment(address);
    address = address & segment->mask;

    if (segment->cached) {
        // todo: read d-cache
    }

    return bus.read(size, address);
}

void core_t::write_data(int size, uint32_t address, uint32_t data) {
    if (cop0.regs[12] & (1 << 16)) {
        return; // isc=1
    }

    if (address == 0xfffe0130) {
        return; // cache control
    }

    auto segment = get_segment(address);
    address = address & segment->mask;

    if (segment->cached) {
        // todo: write d-cache
    }

    return bus.write(size, address, data);
}
