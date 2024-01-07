#pragma once
#include "common.h"
#include "bundle.hpp"
#include "cpu.hpp"

namespace AddImm22 {

static inline void Handle(Ia64Bundle *bundle, Ia64Cpu *cpu, uint64_t slot) {
    uint8_t qp = (slot >> 0) & 63;
    uint8_t r1 = (slot >> 6) & 127;
    uint8_t r3 = (slot >> 20) & 3;
    uint32_t imm7b = (slot >> 13) & 127;
    uint32_t imm5c = (slot >> 22) & 31;
    uint32_t imm9d = (slot >> 27) & 0x1ff;
    uint32_t s = (slot >> 36) & 1;
    uint32_t imm22 = (s << 21) | (imm5c << 16) | (imm9d << 7) | (imm7b);
    uint64_t imm = SignExt(imm22, 22);

    printf("(qp %d) addl r%d = 0x%08x(%ld), r%d\n", qp, r1, imm22, imm, r3);

    if (cpu->regs.pr[qp].val) {
        cpu->regs.CheckTargetRegister(r1);
        cpu->regs.gpr[r1] = imm + cpu->regs.gpr[r3].val;
        cpu->regs.gpr[r1] = cpu->regs.gpr[r3].nat;
    }
}

} // AddImm22