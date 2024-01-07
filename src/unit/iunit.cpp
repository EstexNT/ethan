#include "iunit.hpp"

namespace IUnit {

void Handle(Ia64Bundle *bundle, Ia64Cpu *cpu, uint64_t slot) {
    uint8_t instType = (slot >> 37) & 0xf;
    switch (instType) {
        case InstructionType::Misc_0: {
            enum Op0_X3 {
                Ext6b = 0,
                Chksi = 1,
                MovToPrRot = 2,
                MovToPr = 3,
                BadT_4 = 4,
                BadT_5 = 5,
                BadT_6 = 6,
                MovToB = 7,
            };
            uint8_t x3 = (slot >> 33) & 7;
            switch (x3) {
                case Op0_X3::Ext6b: {
                    switch ((slot >> 31) & 3) {
                        case 3: {
                            switch ((slot >> 27) & 0xf) {
                                case 1: { // mov from b
                                    uint8_t qp = (slot >> 0) & 63;
                                    uint8_t r1 = (slot >> 6) & 127;
                                    uint8_t b2 = (slot >> 13) & 7;

                                    printf("(qp %d) mov r%d = b%d\n", qp, r1, b2);
                                    if (cpu->regs.GetPR(qp)) {
                                        cpu->regs.CheckTargetRegister(r1);
                                        cpu->regs.gpr[r1] = cpu->regs.br[b2].val;
                                        cpu->regs.gpr[r1].nat = false;
                                    }
                                    return;
                                }
                                default: {
                                    fprintf(stderr, "unimpl i unit misc ext 6b 3 %x\n", (slot >> 27) & 0xf);
                                    cpu->halt = true;
                                    return;
                                }
                            }
                            return;
                        }
                        default: {
                            fprintf(stderr, "unimpl i unit misc ext 6b %x\n", (slot >> 31) & 3);
                            cpu->halt = true;
                            return;
                        }
                    }
                    return;
                }
                default: {
                    fprintf(stderr, "unimpl i unit misc x3 %x!!\n", x3);
                    cpu->halt = true;
                    return;
                }
            }
            return;
        }
        case InstructionType::ShiftTestBit_5: {
            uint8_t x2 = (slot >> 34) & 0b11;
            uint8_t x = (slot >> 33) & 1;
            if (x == 0) {
                switch (x2) {
                    default: {
                        fprintf(stderr, "unimpl i unit shift/test bit x=0 x2=%x\n", x2);
                        cpu->halt = true;
                        return;
                    }
                }
            } else {
                switch (x2) {
                    case 3: {
                        uint8_t qp = (slot >> 0) & 0b111111;
                        uint8_t r1 = (slot >> 6) & 0b1111111;
                        uint8_t cpos6b = (slot >> 14) & 0b111111;
                        uint8_t r3 = (slot >> 20) & 0b1111111;
                        uint8_t len6d = (slot >> 27) & 0b111111;
                        uint8_t s = (slot >> 36) & 1;
                        uint64_t imm = s;

                        printf("(qp %d) dep r%d = %d, r%d, %d, %d\n", qp, r1, SignExt(imm, 1), r3, cpos6b, len6d);
                        if (cpu->regs.GetPR(qp)) {
                            cpu->regs.CheckTargetRegister(r1);
                            // imm_form
                            uint64_t tmp_src = SignExt(imm, 1);
                            bool tmp_nat = cpu->regs.gpr[r3].nat;
                            uint8_t tmp_len = len6d;
                            if ((cpos6b + tmp_len) > 64) {
                                tmp_len = 64 - cpos6b;
                            }
                            cpu->regs.gpr[r1] = cpu->regs.gpr[r3].val;
                            
                        }
                        return;
                    }
                    default: {
                        fprintf(stderr, "unimpl i unit shift/test bit x=1 x2=%x\n", x2);
                        cpu->halt = true;
                        return;
                    }
                }
            }
            return;
        }
        case InstructionType::ALUMM_ALU_8: {
            ALU::MMALUHandle(bundle, cpu, slot);
            return;
        }
        case InstructionType::AddImm22_9: {
            AddImm22::Handle(bundle, cpu, slot);
            return;
        }
        default: {
            fprintf(stderr, "unimplemented i unit inst type %x!!\n", instType);
            cpu->halt = true;
            return;
        }
    }
}

}