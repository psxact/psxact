#include "core_t.hpp"
#include "cop0_t.hpp"
#include <stdio.h>
#include <string>
#include <exception>

using namespace r3051;

core_t::core_t(bus_t &bus, cop0_t &cop0)
    : bus(bus)
    , cop0(cop0)
    , regs() {
}

#define uimmediate ((((code) & 0xffff) ^ 0x0000) - 0x0000)
#define simmediate ((((code) & 0xffff) ^ 0x8000) - 0x8000)

#define op_lo (((code) >>  0) & 63)
#define sa    (((code) >>  6) & 31)
#define rd    (((code) >> 11) & 31)
#define rt    (((code) >> 16) & 31)
#define rs    (((code) >> 21) & 31)
#define op_hi (((code) >> 26) & 63)

void core_t::main(void) {
    int64_t mult;
    uint64_t multu;
    uint32_t code;
    uint32_t condition;

    regs.pc = 0xbfc00000;
    regs.next_pc = 0xbfc00004;

    while (1) {
        code = bus.read_code(regs.pc);
        regs.pc = regs.next_pc;
        regs.next_pc += 4;

        switch (op_hi) {
        case 0x00: // special
            switch (op_lo) {
            case 0x00: // sll rd,rt,sa
                regs[rd] = regs[rt] << sa;
                break;

            case 0x02: // srl rd,rt,sa
                regs[rd] = regs[rt] >> sa;
                break;

            case 0x03: // sra rd,rt,sa
                regs[rd] = int32_t(regs[rt]) >> sa;
                break;

            case 0x04: // sllv rd,rt,rs
                regs[rd] = regs[rt] << regs[rs];
                break;

            case 0x06: // srlv rd,rt,rs
                regs[rd] = regs[rt] >> regs[rs];
                break;

            case 0x07: // srav rd,rt,rs
                regs[rd] = int32_t(regs[rt]) >> regs[rs];
                break;

            case 0x08: // jr rs
                regs.next_pc = regs[rs];
                break;

            case 0x09: // jalr rd,rs
                regs[rd] = regs.next_pc;
                regs.next_pc = regs[rs];
                break;

            case 0x0c: // syscall
                printf("syscall $%08x\n", ((code >> 6) & 0xfffff));
                break;

            case 0x0d: // break
                printf("break $%08x\n", ((code >> 6) & 0xfffff));
                break;

            case 0x10: // mfhi rd
                regs[rd] = regs.hi;
                break;

            case 0x11: // mthi rs
                regs.hi = regs[rs];
                break;

            case 0x12: // mflo rd
                regs[rd] = regs.lo;
                break;

            case 0x13: // mtlo rs
                regs.lo = regs[rs];
                break;

            case 0x18: // mult rs,rt
                mult = int64_t(int32_t(regs[rs])) * int64_t(int32_t(regs[rt]));
                regs.lo = uint32_t(mult >> 0);
                regs.hi = uint32_t(mult >> 32);
                break;

            case 0x19: // multu rs,rt
                multu = uint64_t(regs[rs]) * uint64_t(regs[rt]);
                regs.lo = uint32_t(multu >> 0);
                regs.hi = uint32_t(multu >> 32);
                break;

            case 0x1a: // div rs,rt
                if (regs[rt]) {
                    regs.lo = int32_t(regs[rs]) / int32_t(regs[rt]);
                    regs.hi = int32_t(regs[rs]) % int32_t(regs[rt]);
                }
                else {
                    regs.lo = -(int32_t(regs[rs]) >> 31);
                    regs.hi = regs[rs];
                }
                break;

            case 0x1b: // divu rs,rt
                if (regs[rt]) {
                    regs.lo = regs[rs] / regs[rt];
                    regs.hi = regs[rs] % regs[rt];
                }
                else {
                    regs.lo = 0xffffffff;
                    regs.hi = regs[rs];
                }
                break;

            case 0x20: // add rd,rs,rt
                regs[rd] = regs[rs] + regs[rt];
                // todo: overflow exception
                break;

            case 0x21: // addu rd,rs,rt
                regs[rd] = regs[rs] + regs[rt];
                break;

            case 0x22: // sub rd,rs,rt
                regs[rd] = regs[rs] - regs[rt];
                // todo: overflow exception
                break;

            case 0x23: // subu rd,rs,rt
                regs[rd] = regs[rs] - regs[rt];
                break;

            case 0x24: // and rd,rs,rt
                regs[rd] = regs[rs] & regs[rt];
                break;

            case 0x25: // or rd,rs,rt
                regs[rd] = regs[rs] | regs[rt];
                break;

            case 0x26: // xor rd,rs,rt
                regs[rd] = regs[rs] ^ regs[rt];
                break;

            case 0x27: // nor rd,rs,rt
                regs[rd] = ~(regs[rs] | regs[rt]);
                break;

            case 0x2a: // slt rd,rs,rt
                regs[rd] = int32_t(regs[rs]) < int32_t(regs[rt]);
                break;

            case 0x2b: // sltu rd,rs,rt
                regs[rd] = regs[rs] < regs[rt];
                break;

            default:
                printf("unimplemented special instruction: $%02x\n", op_lo);
                return;
            }
            break;

        case 0x01: // reg-imm
            switch (rt) {
            case 0x00: // bltz rs,$nnnn
                if (int32_t(regs[rs]) < 0) {
                    regs.next_pc = regs.pc + (simmediate << 2);
                }
                break;

            case 0x01: // bgez rs,$nnnn
                if (int32_t(regs[rs]) >= 0) {
                    regs.next_pc = regs.pc + (simmediate << 2);
                }
                break;

            case 0x10: // bltzal rs,$nnnn
                condition = int32_t(regs[rs]) < 0;
                regs[31] = regs.next_pc;

                if (condition) {
                    regs.next_pc = regs.pc + (simmediate << 2);
                }
                break;

            case 0x11: // bgezal rs,$nnnn
                condition = int32_t(regs[rs]) >= 0;
                regs[31] = regs.next_pc;

                if (condition) {
                    regs.next_pc = regs.pc + (simmediate << 2);
                }
                break;

            default:
                printf("unimplemented reg-imm instruction: $%02x\n", rt);
                return;
            }
            break;

        case 0x02: // j $3ffffff
            regs.next_pc = (regs.pc & 0xf0000000) | ((code << 2) & 0x0ffffffc);
            break;

        case 0x03: // jal $3ffffff
            regs[31] = regs.next_pc;
            regs.next_pc = (regs.pc & 0xf0000000) | ((code << 2) & 0x0ffffffc);
            break;

        case 0x04: // beq rs,rt,$nnnn
            if (regs[rs] == regs[rt]) {
                regs.next_pc = regs.pc + (simmediate << 2);
            }
            break;

        case 0x05: // bne rs,rt,$nnnn
            if (regs[rs] != regs[rt]) {
                regs.next_pc = regs.pc + (simmediate << 2);
            }
            break;

        case 0x06: // blez rs,$nnnn
            if (int32_t(regs[rs]) <= 0) {
                regs.next_pc = regs.pc + (simmediate << 2);
            }
            break;

        case 0x07: // bgtz rs,$nnnn
            if (int32_t(regs[rs]) > 0) {
                regs.next_pc = regs.pc + (simmediate << 2);
            }
            break;

        case 0x08: // addi rt,rs,$nnnn
            regs[rt] = regs[rs] + simmediate;
            // todo: overflow exception
            break;

        case 0x09: // addiu rt,rs,$nnnn
            regs[rt] = regs[rs] + simmediate;
            break;

        case 0x0a: // slti rt,rs,$nnnn
            regs[rt] = int32_t(regs[rs]) < int32_t(simmediate);
            break;

        case 0x0b: // sltiu rt,rs,$nnnn
            regs[rt] = regs[rs] < simmediate;
            break;

        case 0x0c: // andi rt,rs,$nnnn
            regs[rt] = regs[rs] & uimmediate;
            break;

        case 0x0d: // ori rt,rs,$nnnn
            regs[rt] = regs[rs] | uimmediate;
            break;

        case 0x0e: // xori rt,rs,$nnnn
            regs[rt] = regs[rs] ^ uimmediate;
            break;

        case 0x0f: // lui rt,$nnnn
            regs[rt] = uimmediate << 16;
            break;

        case 0x10: // cop0
            switch (rs) {
            case 0x00: // mfc0 rt,rd
                regs[rt] = cop0.get_register(rd);
                break;

            case 0x04: // mtc0 rt,rd
                cop0.set_register(rd, regs[rt]);
                break;

            default:
                printf("unimplemented cop0 instruction: $%02x\n", rs);
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
            regs[rt] = int8_t(bus.read_data(BYTE, regs[rs] + simmediate));
            break;

        case 0x21: // lh rt,$nnnn(rs)
            regs[rt] = int16_t(bus.read_data(HALF, regs[rs] + simmediate));
            break;

        case 0x22: // lwl rt,$nnnn(rs)
            printf("unimplemented lwl\n");
            return;

        case 0x23: // lw rt,$nnnn(rs)
            regs[rt] = bus.read_data(WORD, regs[rs] + simmediate);
            break;

        case 0x24: // lbu rt,$nnnn(rs)
            regs[rt] = bus.read_data(BYTE, regs[rs] + simmediate);
            break;

        case 0x25: // lhu rt,$nnnn(rs)
            regs[rt] = bus.read_data(HALF, regs[rs] + simmediate);
            break;

        case 0x26: // lwr rt,$nnnn(rs)
            printf("unimplemented lwr\n");
            break;

        case 0x28: // sb rt,$nnnn(rs)
            bus.write_data(BYTE, regs[rs] + simmediate, regs[rt]);
            break;

        case 0x29: // sh rt,$nnnn(rs)
            bus.write_data(HALF, regs[rs] + simmediate, regs[rt]);
            break;

        case 0x2a: // swl rt,$nnnn(rs)
            printf("unimplemented swl\n");
            break;

        case 0x2b: // sw rt,$nnnn(rs)
            bus.write_data(WORD, regs[rs] + simmediate, regs[rt]);
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
            printf("undefined instruction $%02x\n", op_hi);
            return;
        }
    }
}

uint32_t& registers_t::operator[](uint32_t index) {
    return all[index];
}
