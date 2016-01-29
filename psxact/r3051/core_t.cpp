#include "cop0_t.hpp"
#include "core_t.hpp"
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
    regs.pc = 0xbfc00000;
    regs.next_pc = 0xbfc00004;

    while (1) {
        uint32_t code = bus.read_code(regs.pc);
        regs.pc = regs.next_pc;
        regs.next_pc += 4;

        switch (op_hi) {
        case 0x00: // special
            switch (op_lo) {
            case 0x00: // sll rd,rt,sa
                regs.all[rd] = regs.all[rt] << sa;
                break;

            case 0x02: // srl rd,rt,sa
                regs.all[rd] = regs.all[rt] >> sa;
                break;

            case 0x03: // sra rd,rt,sa
                regs.all[rd] = int32_t(regs.all[rt]) >> sa;
                break;

            case 0x04: // sllv rd,rt,rs
                regs.all[rd] = regs.all[rt] << regs.all[rs];
                break;

            case 0x08: // jr rs
                regs.next_pc = regs.all[rs];
                break;

            case 0x09: // jalr rd,rs
                regs.all[rd] = regs.next_pc;
                regs.next_pc = regs.all[rs];
                break;

            case 0x10: // mfhi rd
                regs.all[rd] = regs.hi;
                break;

            case 0x12: // mflo rd
                regs.all[rd] = regs.lo;
                break;

            case 0x1a: // div rs,rt
                if (regs.all[rt]) {
                    regs.lo = int32_t(regs.all[rs]) / int32_t(regs.all[rt]);
                    regs.hi = int32_t(regs.all[rs]) % int32_t(regs.all[rt]);
                }
                else {
                    regs.lo = 0xffffffff;
                    regs.hi = regs.all[rs];
                }
                break;

            case 0x1b: // divu rs,rt
                if (regs.all[rt]) {
                    regs.lo = regs.all[rs] / regs.all[rt];
                    regs.hi = regs.all[rs] % regs.all[rt];
                }
                else {
                    regs.lo = 0;
                    regs.hi = regs.all[rs];
                }
                break;

            case 0x20: // add rd,rs,rt
                regs.all[rd] = regs.all[rs] + regs.all[rt];
                break;

            case 0x21: // addu rd,rs,rt
                regs.all[rd] = regs.all[rs] + regs.all[rt];
                break;

            case 0x23: // subu rd,rs,rt
                regs.all[rd] = regs.all[rs] - regs.all[rt];
                break;

            case 0x24: // and rd,rs,rt
                regs.all[rd] = regs.all[rs] & regs.all[rt];
                break;

            case 0x25: // or rd,rs,rt
                regs.all[rd] = regs.all[rs] | regs.all[rt];
                break;

            case 0x2a: // slt rd,rs,rt
                regs.all[rd] = int32_t(regs.all[rs]) < int32_t(regs.all[rt]);
                break;

            case 0x2b: // sltu rd,rs,rt
                regs.all[rd] = regs.all[rs] < regs.all[rt];
                break;

            default:
                printf("unimplemented special instruction: $%02x\n", op_lo);
                return;
            }
            break;

        case 0x01: // reg-imm
            switch (rt) {
            case 0x00: // bltz rs,$nnnn
                if (int32_t(regs.all[rs]) < 0) {
                    regs.next_pc = regs.pc + (simmediate << 2);
                }
                break;

            case 0x01: // bgez rs,$nnnn
                if (int32_t(regs.all[rs]) >= 0) {
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
            regs.all[31] = regs.next_pc;
            regs.next_pc = (regs.pc & 0xf0000000) | ((code << 2) & 0x0ffffffc);
            break;

        case 0x04: // beq rs,rt,$nnnn
            if (regs.all[rs] == regs.all[rt]) {
                regs.next_pc = regs.pc + (simmediate << 2);
            }
            break;

        case 0x05: // bne rs,rt,$nnnn
            if (regs.all[rs] != regs.all[rt]) {
                regs.next_pc = regs.pc + (simmediate << 2);
            }
            break;

        case 0x06: // blez rs,$nnnn
            if (int32_t(regs.all[rs]) <= 0) {
                regs.next_pc = regs.pc + (simmediate << 2);
            }
            break;

        case 0x07: // bgtz rs,$nnnn
            if (int32_t(regs.all[rs]) > 0) {
                regs.next_pc = regs.pc + (simmediate << 2);
            }
            break;

        case 0x08: // addi rt,rs,$nnnn
            regs.all[rt] = regs.all[rs] + simmediate;
            break;

        case 0x09: // addiu rt,rs,$nnnn
            regs.all[rt] = regs.all[rs] + simmediate;
            break;

        case 0x0a: // slti rt,rs,$nnnn
            regs.all[rt] = int32_t(regs.all[rs]) < int32_t(simmediate);
            break;

        case 0x0b: // sltiu rt,rs,$nnnn
            regs.all[rt] = regs.all[rs] < simmediate;
            break;

        case 0x0c: // andi rt,rs,$nnnn
            regs.all[rt] = regs.all[rs] & uimmediate;
            break;

        case 0x0d: // ori rt,rs,$nnnn
            regs.all[rt] = regs.all[rs] | uimmediate;
            break;

        case 0x0f: // lui rt,$nnnn
            regs.all[rt] = uimmediate << 16;
            break;

        case 0x10: // cop0
            switch (rs) {
            case 0x00: // mfc rt,rd
                regs.all[rt] = cop0.get_register(rd);
                break;

            case 0x04: // mtc rt,rd
                cop0.set_register(rd, regs.all[rt]);
                break;

            default:
                printf("unimplemented cop0 instruction: $%02x\n", rs);
                return;
            }
            break;

        case 0x20: // lb rt,$nnnn(rs)
            regs.all[rt] = int8_t(bus.read_data(BYTE, regs.all[rs] + simmediate));
            break;

        case 0x23: // lw rt,$nnnn(rs)
            regs.all[rt] = bus.read_data(WORD, regs.all[rs] + simmediate);
            break;

        case 0x24: // lbu rt,$nnnn(rs)
            regs.all[rt] = bus.read_data(BYTE, regs.all[rs] + simmediate);
            break;

        case 0x25: // lhu rt,$nnnn(rs)
            regs.all[rt] = bus.read_data(HALF, regs.all[rs] + simmediate);
            break;

        case 0x28: // sb rt,$nnnn(rs)
            bus.write_data(BYTE, regs.all[rs] + simmediate, regs.all[rt]);
            break;

        case 0x29: // sh rt,$nnnn(rs)
            bus.write_data(HALF, regs.all[rs] + simmediate, regs.all[rt]);
            break;

        case 0x2b: // sw rt,$nnnn(rs)
            bus.write_data(WORD, regs.all[rs] + simmediate, regs.all[rt]);
            break;

        default:
            printf("unimplemented instruction: $%02x\n", op_hi);
            return;
        }
    }
}
