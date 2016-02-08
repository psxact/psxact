#include "core_t.hpp"
#include "cop0_t.hpp"
#include "decoder.hpp"
#include <stdio.h>
#include <string>
#include <exception>

using namespace r3051;

core_t::core_t(bus_t &bus, cop0_t &cop0)
    : bus(bus)
    , cop0(cop0)
    , regs() {
}

void core_t::main(void) {
    uint32_t condition;

    regs.pc = 0xbfc00000;
    regs.next_pc = 0xbfc00004;

    while (1) {
        decoder::code = bus.read_code(regs.pc);
        regs.pc = regs.next_pc;
        regs.next_pc += 4;

        switch (decoder::op_hi()) {
        case 0x00: // special
            switch (decoder::op_lo()) {
            case 0x00: // sll rd,rt,sa
                regs[decoder::rd()] = regs[decoder::rt()] << decoder::sa();
                break;

            case 0x02: // srl rd,rt,sa
                regs[decoder::rd()] = regs[decoder::rt()] >> decoder::sa();
                break;

            case 0x03: // sra rd,rt,sa
                regs[decoder::rd()] = int32_t(regs[decoder::rt()]) >> decoder::sa();
                break;

            case 0x04: // sllv rd,rt,rs
                regs[decoder::rd()] = regs[decoder::rt()] << regs[decoder::rs()];
                break;

            case 0x06: // srlv rd,rt,rs
                regs[decoder::rd()] = regs[decoder::rt()] >> regs[decoder::rs()];
                break;

            case 0x07: // srav rd,rt,rs
                regs[decoder::rd()] = int32_t(regs[decoder::rt()]) >> regs[decoder::rs()];
                break;

            case 0x08: // jr rs
                regs.next_pc = regs[decoder::rs()];
                break;

            case 0x09: // jalr rd,rs
                regs[decoder::rd()] = regs.next_pc;
                regs.next_pc = regs[decoder::rs()];
                break;

            case 0x0c: // syscall
                regs.pc = enter_exception(0x08, regs.pc - 4);
                regs.next_pc = regs.pc + 4;
                break;

            case 0x0d: // break
                printf("break $%08x\n", ((decoder::code >> 6) & 0xfffff));
                break;

            case 0x10: // mfhi rd
                regs[decoder::rd()] = regs.hi;
                break;

            case 0x11: // mthi rs
                regs.hi = regs[decoder::rs()];
                break;

            case 0x12: // mflo rd
                regs[decoder::rd()] = regs.lo;
                break;

            case 0x13: // mtlo rs
                regs.lo = regs[decoder::rs()];
                break;

            case 0x18: // mult rs,rt
            {
                int64_t result = int64_t(int32_t(regs[decoder::rs()])) * int64_t(int32_t(regs[decoder::rt()]));
                regs.lo = uint32_t(result >> 0);
                regs.hi = uint32_t(result >> 32);
                break;
            }

            case 0x19: // multu rs,rt
            {
                uint64_t result = uint64_t(regs[decoder::rs()]) * uint64_t(regs[decoder::rt()]);
                regs.lo = uint32_t(result >> 0);
                regs.hi = uint32_t(result >> 32);
                break;
            }

            case 0x1a: // div rs,rt
                if (regs[decoder::rt()]) {
                    regs.lo = int32_t(regs[decoder::rs()]) / int32_t(regs[decoder::rt()]);
                    regs.hi = int32_t(regs[decoder::rs()]) % int32_t(regs[decoder::rt()]);
                }
                else {
                    regs.lo = ((regs[decoder::rs()] >> 30) & 2) - 1;
                    regs.hi = regs[decoder::rs()];
                }
                break;

            case 0x1b: // divu rs,rt
                if (regs[decoder::rt()]) {
                    regs.lo = regs[decoder::rs()] / regs[decoder::rt()];
                    regs.hi = regs[decoder::rs()] % regs[decoder::rt()];
                }
                else {
                    regs.lo = 0xffffffff;
                    regs.hi = regs[decoder::rs()];
                }
                break;

            case 0x20: // add rd,rs,rt
                regs[decoder::rd()] = regs[decoder::rs()] + regs[decoder::rt()];
                // todo: overflow exception
                break;

            case 0x21: // addu rd,rs,rt
                regs[decoder::rd()] = regs[decoder::rs()] + regs[decoder::rt()];
                break;

            case 0x22: // sub rd,rs,rt
                regs[decoder::rd()] = regs[decoder::rs()] - regs[decoder::rt()];
                // todo: overflow exception
                break;

            case 0x23: // subu rd,rs,rt
                regs[decoder::rd()] = regs[decoder::rs()] - regs[decoder::rt()];
                break;

            case 0x24: // and rd,rs,rt
                regs[decoder::rd()] = regs[decoder::rs()] & regs[decoder::rt()];
                break;

            case 0x25: // or rd,rs,rt
                regs[decoder::rd()] = regs[decoder::rs()] | regs[decoder::rt()];
                break;

            case 0x26: // xor rd,rs,rt
                regs[decoder::rd()] = regs[decoder::rs()] ^ regs[decoder::rt()];
                break;

            case 0x27: // nor rd,rs,rt
                regs[decoder::rd()] = ~(regs[decoder::rs()] | regs[decoder::rt()]);
                break;

            case 0x2a: // slt rd,rs,rt
                regs[decoder::rd()] = int32_t(regs[decoder::rs()]) < int32_t(regs[decoder::rt()]);
                break;

            case 0x2b: // sltu rd,rs,rt
                regs[decoder::rd()] = regs[decoder::rs()] < regs[decoder::rt()];
                break;

            default:
                printf("unimplemented special instruction: $%02x\n", decoder::op_lo());
                return;
            }
            break;

        case 0x01: // reg-imm
            switch (decoder::rt()) {
            case 0x00: // bltz rs,$nnnn
                if (int32_t(regs[decoder::rs()]) < 0) {
                    regs.next_pc = regs.pc + (decoder::simmediate() << 2);
                }
                break;

            case 0x01: // bgez rs,$nnnn
                if (int32_t(regs[decoder::rs()]) >= 0) {
                    regs.next_pc = regs.pc + (decoder::simmediate() << 2);
                }
                break;

            case 0x10: // bltzal rs,$nnnn
                condition = int32_t(regs[decoder::rs()]) < 0;
                regs[31] = regs.next_pc;

                if (condition) {
                    regs.next_pc = regs.pc + (decoder::simmediate() << 2);
                }
                break;

            case 0x11: // bgezal rs,$nnnn
                condition = int32_t(regs[decoder::rs()]) >= 0;
                regs[31] = regs.next_pc;

                if (condition) {
                    regs.next_pc = regs.pc + (decoder::simmediate() << 2);
                }
                break;

            default:
                printf("unimplemented reg-imm instruction: $%02x\n", decoder::rt());
                return;
            }
            break;

        case 0x02: // j $3ffffff
            regs.next_pc = (regs.pc & 0xf0000000) | ((decoder::code << 2) & 0x0ffffffc);
            break;

        case 0x03: // jal $3ffffff
            regs[31] = regs.next_pc;
            regs.next_pc = (regs.pc & 0xf0000000) | ((decoder::code << 2) & 0x0ffffffc);
            break;

        case 0x04: // beq rs,rt,$nnnn
            if (regs[decoder::rs()] == regs[decoder::rt()]) {
                regs.next_pc = regs.pc + (decoder::simmediate() << 2);
            }
            break;

        case 0x05: // bne rs,rt,$nnnn
            if (regs[decoder::rs()] != regs[decoder::rt()]) {
                regs.next_pc = regs.pc + (decoder::simmediate() << 2);
            }
            break;

        case 0x06: // blez rs,$nnnn
            if (int32_t(regs[decoder::rs()]) <= 0) {
                regs.next_pc = regs.pc + (decoder::simmediate() << 2);
            }
            break;

        case 0x07: // bgtz rs,$nnnn
            if (int32_t(regs[decoder::rs()]) > 0) {
                regs.next_pc = regs.pc + (decoder::simmediate() << 2);
            }
            break;

        case 0x08: // addi rt,rs,$nnnn
            regs[decoder::rt()] = regs[decoder::rs()] + decoder::simmediate();
            // todo: overflow exception
            break;

        case 0x09: // addiu rt,rs,$nnnn
            regs[decoder::rt()] = regs[decoder::rs()] + decoder::simmediate();
            break;

        case 0x0a: // slti rt,rs,$nnnn
            regs[decoder::rt()] = int32_t(regs[decoder::rs()]) < int32_t(decoder::simmediate());
            break;

        case 0x0b: // sltiu rt,rs,$nnnn
            regs[decoder::rt()] = regs[decoder::rs()] < decoder::simmediate();
            break;

        case 0x0c: // andi rt,rs,$nnnn
            regs[decoder::rt()] = regs[decoder::rs()] & decoder::uimmediate();
            break;

        case 0x0d: // ori rt,rs,$nnnn
            regs[decoder::rt()] = regs[decoder::rs()] | decoder::uimmediate();
            break;

        case 0x0e: // xori rt,rs,$nnnn
            regs[decoder::rt()] = regs[decoder::rs()] ^ decoder::uimmediate();
            break;

        case 0x0f: // lui rt,$nnnn
            regs[decoder::rt()] = decoder::uimmediate() << 16;
            break;

        case 0x10: // cop0
            switch (decoder::rs()) {
            case 0x00: // mfc0 rt,rd
                regs[decoder::rt()] = cop0.get_register(decoder::rd());
                break;

            case 0x04: // mtc0 rt,rd
                cop0.set_register(decoder::rd(), regs[decoder::rt()]);
                break;

            case 0x10: // rfe
                printf("unimplemented rfe\n");
                break;

            default:
                printf("unimplemented cop0 instruction: $%02x\n", decoder::rs());
                return;
            }
            break;

        case 0x11: // cop1
            printf("unimplemented cop1\n");
            break;

        case 0x12: // cop2
            printf("unimplemented cop2\n");
            break;

        case 0x13: // cop3
            printf("unimplemented cop3\n");
            break;

        case 0x20: // lb rt,$nnnn(rs)
            regs[decoder::rt()] = int8_t(bus.read_data(BYTE, regs[decoder::rs()] + decoder::simmediate()));
            break;

        case 0x21: // lh rt,$nnnn(rs)
            regs[decoder::rt()] = int16_t(bus.read_data(HALF, regs[decoder::rs()] + decoder::simmediate()));
            break;

        case 0x22: // lwl rt,$nnnn(rs)
            printf("unimplemented lwl\n");
            return;

        case 0x23: // lw rt,$nnnn(rs)
            regs[decoder::rt()] = bus.read_data(WORD, regs[decoder::rs()] + decoder::simmediate());
            break;

        case 0x24: // lbu rt,$nnnn(rs)
            regs[decoder::rt()] = bus.read_data(BYTE, regs[decoder::rs()] + decoder::simmediate());
            break;

        case 0x25: // lhu rt,$nnnn(rs)
            regs[decoder::rt()] = bus.read_data(HALF, regs[decoder::rs()] + decoder::simmediate());
            break;

        case 0x26: // lwr rt,$nnnn(rs)
            printf("unimplemented lwr\n");
            break;

        case 0x28: // sb rt,$nnnn(rs)
            bus.write_data(BYTE, regs[decoder::rs()] + decoder::simmediate(), regs[decoder::rt()]);
            break;

        case 0x29: // sh rt,$nnnn(rs)
            bus.write_data(HALF, regs[decoder::rs()] + decoder::simmediate(), regs[decoder::rt()]);
            break;

        case 0x2a: // swl rt,$nnnn(rs)
            printf("unimplemented swl\n");
            break;

        case 0x2b: // sw rt,$nnnn(rs)
            bus.write_data(WORD, regs[decoder::rs()] + decoder::simmediate(), regs[decoder::rt()]);
            break;

        case 0x2e: // swr rt,$nnnn(rs)
            printf("unimplemented swr\n");
            break;

        case 0x30: // lwc0 rt,$nnnn(rs)
        case 0x31: // lwc1 rt,$nnnn(rs)
        case 0x32: // lwc2 rt,$nnnn(rs)
        case 0x33: // lwc3 rt,$nnnn(rs)
            printf("unimplemented lwc\n");
            break;

        case 0x38: // swc0 rt,$nnnn(rs)
        case 0x39: // swc1 rt,$nnnn(rs)
        case 0x3a: // swc2 rt,$nnnn(rs)
        case 0x3b: // swc3 rt,$nnnn(rs)
            printf("unimplemented swc\n");
            break;

        default:
            printf("undefined instruction $%02x\n", decoder::op_hi());
            return;
        }
    }
}

uint32_t& registers_t::operator[](uint32_t index) {
    return all[index];
}

uint32_t core_t::enter_exception(uint32_t excode, uint32_t epc) {
    uint32_t sr = cop0.get_register(12);
    sr = (sr & ~0x3f) | ((sr << 2) & 0x3f);

    uint32_t cause = cop0.get_register(13);
    cause = (cause & ~0x3f) | ((excode << 2) & 0x3f);

    cop0.set_register(12, sr);
    cop0.set_register(13, cause);
    cop0.set_register(14, epc);

    return (sr & (1 << 22))
        ? 0xbfc00180
        : 0x80000080
        ;
}

uint32_t core_t::leave_exception() {
    uint32_t sr = cop0.get_register(12);
    sr = (sr & ~0xf) | ((sr >> 2) & 0xf);

    cop0.set_register(12, sr);

    return cop0.get_register(14);
}
