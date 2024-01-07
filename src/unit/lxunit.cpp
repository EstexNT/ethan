#include "lxunit.hpp"

namespace LXUnit {

void Handle(Ia64Bundle *bundle, Ia64Cpu *cpu, uint64_t slot1, uint64_t slot2) {
    uint8_t instType = (slot2 >> 37) & 0xf;
    switch (instType) {
        case InstructionType::Movl_6: {
            uint8_t vc = (slot2 >> 20) & 1;
            if (vc == 0) {
                uint8_t qp = (slot2 >> 0) & 63;
                uint8_t r1 = (slot2 >> 6) & 127;
                uint64_t imm41 = slot1;
                uint64_t imm7b = (slot2 >> 13) & 127;
                uint64_t ic = (slot2 >> 21) & 1;
                uint64_t imm5c = (slot2 >> 22) & 31;
                uint64_t imm9d = (slot2 >> 27) & 0x1ff;
                uint64_t i = (slot2 >> 36) & 1;
                // wow okay
                uint64_t imm64 = (i << 63) | (imm41 << 22) | (ic << 21) 
                                | (imm5c << 16) | (imm9d << 7) | (imm7b);

                printf("(qp %d) movl r%d = 0x%016" PRIx64 "\n", qp, r1, imm64);
                if (cpu->regs.pr[qp].val) {
                    cpu->regs.CheckTargetRegister(r1);
                    cpu->regs.gpr[r1] = imm64;
                    cpu->regs.gpr[r1] = false;
                }
            } else {
                fprintf(stderr, "reserved lxunit inst movl vc=1\n");
                cpu->halt = true;
            }

            return;
        }
        default: {
            fprintf(stderr, "unimpl lxunit inst type %x\n", instType);
            cpu->halt = true;
            return;
        }
    }
}

} // LXUnit
