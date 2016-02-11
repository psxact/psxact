#include "cpu_core.hpp"
#include "decoder.hpp"
#include "../bus.hpp"
#include "../utility.hpp"

using namespace r3051;

core_t::core_t(bus_t &bus)
    : bus(bus)
    , cop0()
    , regs() {
}

void core_t::main() {
    bool condition;

    regs.pc = 0xbfc00000;
    regs.next_pc = 0xbfc00004;

    while (1) {
        decoder::code = read_code();

        switch (decoder::op_hi()) {
        case 0x00: // special
            switch (decoder::op_lo()) {
            case 0x00: // sll rd,rt,sa
                regs.u[decoder::rd()] = regs.u[decoder::rt()] << decoder::sa();
                continue;

            case 0x02: // srl rd,rt,sa
                regs.u[decoder::rd()] = regs.u[decoder::rt()] >> decoder::sa();
                continue;

            case 0x03: // sra rd,rt,sa
                regs.i[decoder::rd()] = regs.i[decoder::rt()] >> decoder::sa();
                continue;

            case 0x04: // sllv rd,rt,rs
                regs.u[decoder::rd()] = regs.u[decoder::rt()] << regs.u[decoder::rs()];
                continue;

            case 0x06: // srlv rd,rt,rs
                regs.u[decoder::rd()] = regs.u[decoder::rt()] >> regs.u[decoder::rs()];
                continue;

            case 0x07: // srav rd,rt,rs
                regs.i[decoder::rd()] = regs.i[decoder::rt()] >> regs.u[decoder::rs()];
                continue;

            case 0x08: // jr rs
                regs.next_pc = regs.u[decoder::rs()];
                continue;

            case 0x09: // jalr rd,rs
                regs.u[decoder::rd()] = regs.next_pc;
                regs.next_pc = regs.u[decoder::rs()];
                continue;

            case 0x0c: // syscall
                enter_exception(0x08, regs.pc - 4);
                continue;

            case 0x0d: // break
                enter_exception(0x09, regs.pc - 4);
                continue;

            case 0x10: // mfhi rd
                regs.u[decoder::rd()] = regs.hi;
                continue;

            case 0x11: // mthi rs
                regs.hi = regs.u[decoder::rs()];
                continue;

            case 0x12: // mflo rd
                regs.u[decoder::rd()] = regs.lo;
                continue;

            case 0x13: // mtlo rs
                regs.lo = regs.u[decoder::rs()];
                continue;

            case 0x18: // mult rs,rt
            {
                int64_t result = int64_t(regs.i[decoder::rs()]) * int64_t(regs.i[decoder::rt()]);
                regs.lo = uint32_t(result >> 0);
                regs.hi = uint32_t(result >> 32);
                continue;
            }

            case 0x19: // multu rs,rt
            {
                uint64_t result = uint64_t(regs.u[decoder::rs()]) * uint64_t(regs.u[decoder::rt()]);
                regs.lo = uint32_t(result >> 0);
                regs.hi = uint32_t(result >> 32);
                continue;
            }

            case 0x1a: // div rs,rt
                if (regs.u[decoder::rt()]) {
                    regs.lo = regs.i[decoder::rs()] / regs.i[decoder::rt()];
                    regs.hi = regs.i[decoder::rs()] % regs.i[decoder::rt()];
                }
                else {
                    regs.lo = ((regs.u[decoder::rs()] >> 30) & 2) - 1;
                    regs.hi = regs.u[decoder::rs()];
                }
                continue;

            case 0x1b: // divu rs,rt
                if (regs.u[decoder::rt()]) {
                    regs.lo = regs.u[decoder::rs()] / regs.u[decoder::rt()];
                    regs.hi = regs.u[decoder::rs()] % regs.u[decoder::rt()];
                }
                else {
                    regs.lo = 0xffffffff;
                    regs.hi = regs.u[decoder::rs()];
                }
                continue;

            case 0x20: // add rd,rs,rt
                regs.u[decoder::rd()] = regs.u[decoder::rs()] + regs.u[decoder::rt()];
                // todo: overflow exception
                continue;

            case 0x21: // addu rd,rs,rt
                regs.u[decoder::rd()] = regs.u[decoder::rs()] + regs.u[decoder::rt()];
                continue;

            case 0x22: // sub rd,rs,rt
                regs.u[decoder::rd()] = regs.u[decoder::rs()] - regs.u[decoder::rt()];
                // todo: overflow exception
                continue;

            case 0x23: // subu rd,rs,rt
                regs.u[decoder::rd()] = regs.u[decoder::rs()] - regs.u[decoder::rt()];
                continue;

            case 0x24: // and rd,rs,rt
                regs.u[decoder::rd()] = regs.u[decoder::rs()] & regs.u[decoder::rt()];
                continue;

            case 0x25: // or rd,rs,rt
                regs.u[decoder::rd()] = regs.u[decoder::rs()] | regs.u[decoder::rt()];
                continue;

            case 0x26: // xor rd,rs,rt
                regs.u[decoder::rd()] = regs.u[decoder::rs()] ^ regs.u[decoder::rt()];
                continue;

            case 0x27: // nor rd,rs,rt
                regs.u[decoder::rd()] = ~(regs.u[decoder::rs()] | regs.u[decoder::rt()]);
                continue;

            case 0x2a: // slt rd,rs,rt
                regs.u[decoder::rd()] = regs.i[decoder::rs()] < regs.i[decoder::rt()];
                continue;

            case 0x2b: // sltu rd,rs,rt
                regs.u[decoder::rd()] = regs.u[decoder::rs()] < regs.u[decoder::rt()];
                continue;
            }
            break;

        case 0x01: // reg-imm
            switch (decoder::rt()) {
            case 0x00: // bltz rs,$nnnn
                if (regs.i[decoder::rs()] < 0) {
                    regs.next_pc = regs.pc + (decoder::iconst() << 2);
                }
                continue;

            case 0x01: // bgez rs,$nnnn
                if (regs.i[decoder::rs()] >= 0) {
                    regs.next_pc = regs.pc + (decoder::iconst() << 2);
                }
                continue;

            case 0x10: // bltzal rs,$nnnn
                condition = regs.i[decoder::rs()] < 0;
                regs.u[31] = regs.next_pc;

                if (condition) {
                    regs.next_pc = regs.pc + (decoder::iconst() << 2);
                }
                continue;

            case 0x11: // bgezal rs,$nnnn
                condition = regs.i[decoder::rs()] >= 0;
                regs.u[31] = regs.next_pc;

                if (condition) {
                    regs.next_pc = regs.pc + (decoder::iconst() << 2);
                }
                continue;
            }
            break;

        case 0x02: // j $3ffffff
            regs.next_pc = (regs.pc & 0xf0000000) | ((decoder::code << 2) & 0x0ffffffc);
            continue;

        case 0x03: // jal $3ffffff
            regs.u[31] = regs.next_pc;
            regs.next_pc = (regs.pc & 0xf0000000) | ((decoder::code << 2) & 0x0ffffffc);
            continue;

        case 0x04: // beq rs,rt,$nnnn
            if (regs.u[decoder::rs()] == regs.u[decoder::rt()]) {
                regs.next_pc = regs.pc + (decoder::iconst() << 2);
            }
            continue;

        case 0x05: // bne rs,rt,$nnnn
            if (regs.u[decoder::rs()] != regs.u[decoder::rt()]) {
                regs.next_pc = regs.pc + (decoder::iconst() << 2);
            }
            continue;

        case 0x06: // blez rs,$nnnn
            if (regs.i[decoder::rs()] <= 0) {
                regs.next_pc = regs.pc + (decoder::iconst() << 2);
            }
            continue;

        case 0x07: // bgtz rs,$nnnn
            if (regs.i[decoder::rs()] > 0) {
                regs.next_pc = regs.pc + (decoder::iconst() << 2);
            }
            continue;

        case 0x08: // addi rt,rs,$nnnn
            regs.u[decoder::rt()] = regs.u[decoder::rs()] + decoder::iconst();
            // todo: overflow exception
            continue;

        case 0x09: // addiu rt,rs,$nnnn
            regs.u[decoder::rt()] = regs.u[decoder::rs()] + decoder::iconst();
            continue;

        case 0x0a: // slti rt,rs,$nnnn
            regs.u[decoder::rt()] = regs.i[decoder::rs()] < decoder::iconst();
            continue;

        case 0x0b: // sltiu rt,rs,$nnnn
            regs.u[decoder::rt()] = regs.u[decoder::rs()] < uint32_t(decoder::iconst());
            continue;

        case 0x0c: // andi rt,rs,$nnnn
            regs.u[decoder::rt()] = regs.u[decoder::rs()] & decoder::uconst();
            continue;

        case 0x0d: // ori rt,rs,$nnnn
            regs.u[decoder::rt()] = regs.u[decoder::rs()] | decoder::uconst();
            continue;

        case 0x0e: // xori rt,rs,$nnnn
            regs.u[decoder::rt()] = regs.u[decoder::rs()] ^ decoder::uconst();
            continue;

        case 0x0f: // lui rt,$nnnn
            regs.u[decoder::rt()] = decoder::uconst() << 16;
            continue;

        case 0x10: // cop0
            switch (decoder::rs()) {
            case 0x00: // mfc0 rt,rd
                regs.u[decoder::rt()] = cop0.regs[decoder::rd()];
                continue;

            case 0x04: // mtc0 rt,rd
                cop0.regs[decoder::rd()] = regs.u[decoder::rt()];
                continue;

            case 0x10:
                switch (decoder::op_lo()) {
                case 0x01: utility::debug("unimplemented tlbr\n"); return;
                case 0x02: utility::debug("unimplemented tlbwi\n"); return;
                case 0x06: utility::debug("unimplemented tblwr\n"); return;
                case 0x08: utility::debug("unimplemented tlbp\n"); return;
                case 0x10: // rfe
                    leave_exception();
                    continue;
                }
                break;
            }
            break;

        case 0x11: // cop1
            utility::debug("unimplemented cop1\n");
            break;

        case 0x12: // cop2
            utility::debug("unimplemented cop2\n");
            break;

        case 0x13: // cop3
            utility::debug("unimplemented cop3\n");
            break;

        case 0x20: // lb rt,$nnnn(rs)
            regs.i[decoder::rt()] = int8_t(read_data(BYTE, regs.u[decoder::rs()] + decoder::iconst()));
            continue;

        case 0x21: // lh rt,$nnnn(rs)
            regs.i[decoder::rt()] = int16_t(read_data(HALF, regs.u[decoder::rs()] + decoder::iconst()));
            continue;

        case 0x22: // lwl rt,$nnnn(rs)
            utility::debug("unimplemented lwl\n");
            continue;

        case 0x23: // lw rt,$nnnn(rs)
            regs.u[decoder::rt()] = read_data(WORD, regs.u[decoder::rs()] + decoder::iconst());
            continue;

        case 0x24: // lbu rt,$nnnn(rs)
            regs.u[decoder::rt()] = read_data(BYTE, regs.u[decoder::rs()] + decoder::iconst());
            continue;

        case 0x25: // lhu rt,$nnnn(rs)
            regs.u[decoder::rt()] = read_data(HALF, regs.u[decoder::rs()] + decoder::iconst());
            continue;

        case 0x26: // lwr rt,$nnnn(rs)
            utility::debug("unimplemented lwr\n");
            break;

        case 0x28: // sb rt,$nnnn(rs)
            write_data(BYTE, regs.u[decoder::rs()] + decoder::iconst(), regs.u[decoder::rt()]);
            continue;

        case 0x29: // sh rt,$nnnn(rs)
            write_data(HALF, regs.u[decoder::rs()] + decoder::iconst(), regs.u[decoder::rt()]);
            continue;

        case 0x2a: // swl rt,$nnnn(rs)
            utility::debug("unimplemented swl\n");
            break;

        case 0x2b: // sw rt,$nnnn(rs)
            write_data(WORD, regs.u[decoder::rs()] + decoder::iconst(), regs.u[decoder::rt()]);
            continue;

        case 0x2e: // swr rt,$nnnn(rs)
            utility::debug("unimplemented swr\n");
            break;

        case 0x30: // lwc0 rt,$nnnn(rs)
        case 0x31: // lwc1 rt,$nnnn(rs)
        case 0x32: // lwc2 rt,$nnnn(rs)
        case 0x33: // lwc3 rt,$nnnn(rs)
            utility::debug("unimplemented lwc\n");
            break;

        case 0x38: // swc0 rt,$nnnn(rs)
        case 0x39: // swc1 rt,$nnnn(rs)
        case 0x3a: // swc2 rt,$nnnn(rs)
        case 0x3b: // swc3 rt,$nnnn(rs)
            utility::debug("unimplemented swc\n");
            break;
        }
    }
}

void core_t::enter_exception(uint32_t excode, uint32_t epc) {
    uint32_t sr = cop0.regs[12];
    sr = (sr & ~0x3f) | ((sr << 2) & 0x3f);

    uint32_t cause = cop0.regs[13];
    cause = (cause & ~0x3f) | ((excode << 2) & 0x3f);

    cop0.regs[12] = sr;
    cop0.regs[13] = cause;
    cop0.regs[14] = epc;

    regs.pc = (sr & (1 << 22))
        ? 0xbfc00180
        : 0x80000080
        ;

    regs.next_pc = regs.pc + 4;
}

void core_t::leave_exception() {
    uint32_t sr = cop0.regs[12];
    sr = (sr & ~0xf) | ((sr >> 2) & 0xf);

    cop0.regs[12] = sr;
}
