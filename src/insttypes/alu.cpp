#include "alu.hpp"

namespace ALU {

#define DEFINEA3VARS \
    uint8_t s = (slot >> 36) & 1; \
    uint8_t r3 = (slot >> 20) & 127; \
    uint8_t imm7b = (slot >> 13) & 127; \
    uint8_t r1 = (slot >> 6) & 127; \
    uint8_t qp = (slot >> 0) & 63; \
    uint8_t imm8 = (s << 7) | (imm7b); \
    uint64_t imm = SignExt(imm8, 8);


void MMALUHandle(Ia64Bundle *bundle, Ia64Cpu *cpu, uint64_t slot) {
    // TODO dire need of tables!! 
    uint8_t x2a = (slot >> 34) & 3;
    uint8_t ve = (slot >> 33) & 1;
    switch (x2a) {
        case 0: {
            if (ve == 0) {
                uint8_t x4 = (slot >> 29) & 0xf;
                uint8_t x2b = (slot >> 27) & 3;
                if ((x4 == 0xb) && (x2b == 3)) {
                    DEFINEA3VARS;
                    printf("(qp %d) xor r%d = %ld(%d), r%d\n", qp, r1, imm, imm8, r3);
                    if (cpu->branched) {
                        return;
                    }
                    if (cpu->regs.pr[qp].val) {
                        cpu->regs.CheckTargetRegister(r1);
                        uint64_t tmp_src = imm;
                        cpu->regs.gpr[r1] = tmp_src ^ cpu->regs.gpr[r3].val;
                        cpu->regs.gpr[r1] = cpu->regs.gpr[r3].nat;
                    }
                    return;
                } else {
                    fprintf(stderr, "unimpl i unit alumm alu x2a 0 x4 %x x2b %x\n", x4, x2b);
                    cpu->halt = true;
                    return;
                }
            } else {
                fprintf(stderr, "bad i unit alumm alu x2a 0 ve 1\n");
                cpu->halt = true;
                return;
            }
            return;
        }
        case 2: {
            if (ve == 0) {
                uint8_t qp = (slot >> 0) & 63;
                uint8_t r1 = (slot >> 6) & 127;
                uint8_t imm7b = (slot >> 13) & 127;
                uint8_t r3 = (slot >> 20) & 127;
                uint8_t imm6d = (slot >> 27) & 63;
                uint8_t sign = (slot >> 36) & 1;
                uint16_t imm = Imm14(sign, imm6d, imm7b);
                uint64_t imma = SignExt(imm, 14);
                printf("(qp %d) adds r%d = %ld, r%d\n", qp, r1, imma, r3);
                if (cpu->regs.pr[qp].val) {
                    cpu->regs.CheckTargetRegister(r1);
                    cpu->regs.gpr[r1] = imma + cpu->regs.gpr[r3].val;
                    cpu->regs.gpr[r1] = cpu->regs.gpr[r3].nat;
                }
                return;
            } else {
                fprintf(stderr, "bad i unit alumm alu x2a 2 ve 1\n");
                cpu->halt = true;
                return;
            }
        }
        default: {
            fprintf(stderr, "unimpl i unit alumm alu x2a %x\n", x2a);
            cpu->halt = true;
            return;
        }
    }
}

} // ALU
