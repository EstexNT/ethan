#include "munit.hpp"

namespace MUnit {
#define DEFINEM3VARS \
    uint8_t qp = (slot >> 0) & 63; \
    uint8_t r1 = (slot >> 6) & 127; \
    uint8_t imm7b = (slot >> 13) & 127; \
    uint8_t r3 = (slot >> 20) & 127; \
    uint8_t i = (slot >> 27) & 1; \
    uint8_t hint = (slot >> 28) & 3; \
    uint8_t s = (slot >> 36) & 1; \
    uint16_t imm9 = (s << 8) | (i << 7) | imm7b; 


static inline void CheckTargetRegisterSof(uint64_t r1, uint64_t newsof) {
    debugprintf("TODO: check_target_register_sof(%lx, %lx)\n", r1, newsof);
    // If r1 targets an out-of-frame stacked register (as defined by the newsof parameter), 
    // an illegal operation fault is delivered, and this function does not return.
    // what does ^^^ mean??
}

void Handle(Ia64Bundle *bundle, Ia64Cpu *cpu, uint64_t slot) {
    //printf("%016lx\n", slot);
    uint8_t instType = (slot >> 37) & 0xf;

    switch (instType) {
        case InstructionType::SysMem_Mgmt_0: {
            uint8_t x3 = (slot >> 33) & 7;
            enum Type0_X3 {
                SysMemMan42bit_0 = 0,
                BadT_1 = 1,
                BadT_2 = 2,
                BadT_3 = 3,
                ChkANcInt_4 = 4,
                ChkAClrInt_5 = 5,
                ChkANcFp_6 = 6,
                ChkAClrFp_7 = 7,
            };
            switch (x3) {
                case Type0_X3::SysMemMan42bit_0: {
                    uint8_t x4 = (slot >> 27) & 15;
                    uint8_t x2 = (slot >> 31) & 3;
                    // TODO: replace the switch statements with something
                    // like a table of funcs
                    // better yet is a JIT but not now :3
                    switch (x4) {
                        case 1: {
                            switch (x2) {
                                case 0: {
                                    uint8_t qp = (slot >> 0) & 63;
                                    uint32_t imm20a = (slot >> 6) & 0xfffff;
                                    uint8_t i = (slot >> 36) & 1;
                                    uint32_t imm21 = (i << 20) | (imm20a);
                                    printf("(qp %d) nop.m %08x\n", qp, imm21);
                                    if (cpu->regs.pr[qp].val) {
                                        // no operation
                                    }
                                    return;
                                }
                                default: {
                                    fprintf(stderr, "unimpl munit sysmem mgmt 4-2 bitext x4=1 x2 %x\n", x2);
                                    cpu->halt = true;
                                    return;
                                }
                            }
                            return;
                        }
                        default: {
                            fprintf(stderr, "unimpl munit sysmem mgmt 4-2 bitext x4 %x\n", x4);
                            cpu->halt = true;
                            return;
                        }
                    }
                    return;
                }
                default: {
                    fprintf(stderr, "unimpl munit sysmem mgmt x3 %x\n", x3);
                    cpu->halt = true;
                    return;
                }
            }
            return;
        }
        case InstructionType::SysMem_Mgmt_1: {
            uint8_t x3 = (slot >> 33) & 7;
            enum Type1_X3 {
                SysMemMan6bit_0 = 0,
                Chksm_1 = 1,
                BadT_2  = 2,
                Chks_3  = 3,
                BadT_4  = 4,
                BadT_5  = 5,
                Alloc_6 = 6,
                BadT_7  = 7,
            };
            switch (x3) {
                case Type1_X3::Alloc_6: {
                    Ia64Regs::Cfm *cfm = &cpu->regs.cfm;
                    uint8_t qp = (slot >> 0) & 63;
                    uint8_t r1 = (slot >> 6) & 127;
                    uint8_t sof = (slot >> 13) & 127;
                    uint8_t sol = (slot >> 20) & 127;
                    uint8_t sor = (slot >> 27) & 15;
                    printf("(qp %d) alloc r%d = ar.pfs, %d, %d, %d\n", qp, r1, sof, sol, sor);
                    if (cpu->branched) {
                        return;
                    }
                    CheckTargetRegisterSof(r1, sof);

                    if ((sof > 96) || ((sor << 3) > sof) || (sol > sof) || (qp != 0)) {
                        cpu->IllegalOperationFault();
                    }
                    if ((sor != cfm->sor) && ((cfm->rrb.gr != 0) || 
                                                    (cfm->rrb.fr != 0) ||
                                                    (cfm->rrb.pr !=0))) {
                        cpu->ReservedRegisterFieldFault();
                    }

                    cpu->alat.FrameUpdate(0, sof - cfm->sof);
                    cpu->rse.NewFrame(cfm->sof, sof);
                    cfm->sof = sof;
                    cfm->sol = sol;
                    cfm->sor = sor;
                    cpu->regs.gpr[r1] = cpu->regs.ar[Ia64Regs::Ar::Type::PFS].val;
                    cpu->regs.gpr[r1] = false;
                    return;
                }
                default: {
                    fprintf(stderr, "unimplemented m unit sys mem mgmt x3 %02x!!\n", x3);
                    cpu->halt = true;
                    return;
                }
            }
            return;
        }
        case InstructionType::IntLd_RegGetf_4: {
            uint8_t m = (slot >> 36) & 1;
            uint8_t x = (slot >> 27) & 1;
            if ((m == 0) && (x == 0)) {
                uint8_t x6 = (slot >> 30) & 3;
                switch ((slot >> 32) & 0xf) {
                    // TODO: refactor into a 2d array
                    // IntLd_RegGetf_m0x0[BITS35_32][x6]
                    case 0: {
                        switch (x6) {
                            case 3: {
                                uint8_t qp = (slot >> 0) & 63;
                                uint8_t r1 = (slot >> 6) & 127;
                                uint8_t r3 = (slot >> 20) & 127;
                                uint8_t hint = (slot >> 28) & 3;

                                printf("(qp %d) ld8%s r%d = [r%d]\n", qp, Hint::GetHintStr(hint), r1, r3);
                                if (cpu->branched) {
                                    return;
                                }
                                if (cpu->regs.pr[qp].val) {
                                    // page 146 
                                    uint8_t size = 8;
                                    uint8_t itype = 0;//READ
                                    // ittype = READ
                                    cpu->regs.CheckTargetRegister(r1);
                                    if (cpu->regs.gpr[r3].nat) {
                                        cpu->RegisterNatConsumptionFault(itype);
                                    }
                                    //paddr = tlb_translate(GR[r3], size, itype, PSR.cpl, &mattr, &defer);
                                    uint64_t paddr = cpu->regs.gpr[r3].val;

                                    uint64_t val = 0;
                                    
                                    Memory::ReadAt<uint64_t>(&val, paddr);
                                    //printf("read %lx at %lx\n", val, paddr);
                                    cpu->regs.gpr[r1] = ZeroExt(val, size * 8);
                                    cpu->regs.gpr[r1] = false;
                                }
                                return;
                            }
                            default: {
                                fprintf(stderr, "unimpl munit intld +reg m0x0 b32=0 %x\n", x6);
                                cpu->halt = true;
                                return;
                            }
                        }
                        return;
                    }
                    case 0xc: {
                        switch (x6) {
                            case 3: {
                                uint8_t qp = (slot >> 0) & 0b111111;
                                uint8_t r2 = (slot >> 13) & 0b1111111;
                                uint8_t r3 = (slot >> 20) & 0b1111111;
                                uint8_t hint = (slot >> 28) & 0b11;

                                printf("(qp %d) st8%s [r%d] = r%d\n", qp, Hint::GetHintStr(hint), r3, r2);
                                if (cpu->branched) {
                                    return;
                                }
                                if (cpu->regs.pr[qp].val) {
                                    // page 233
                                    uint8_t size = 8;
                                    uint8_t otype = 0; //UNORDERED
                                    if (cpu->regs.gpr[r3].nat || cpu->regs.gpr[r2].nat) {
                                        cpu->RegisterNatConsumptionFault(1); // WRITE
                                    }
                                    //paddr = tlb_translate()
                                    Ia64Addr paddr = cpu->regs.gpr[r3].val;
                                    Memory::WriteAt<uint64_t>(&cpu->regs.gpr[r2].val, paddr);
                                    //alat_inval_multiple_entries(paddr, size);

                                }
                                return;
                            }
                            default: {
                                fprintf(stderr, "unimpl munit intld +reg m0x0 b32=0xc %x\n", x6);
                                cpu->halt = true;
                                return;
                            }
                        }
                        return;
                    }
                    default: {
                        fprintf(stderr, "unimpl m unit intld +reg m0x0 %x\n", uint32_t((slot >> 32) & 0xf));
                        cpu->halt = true;
                        return;
                    }
                }
            } else {
                fprintf(stderr, "unimpl m unit intld +reg m %x x %x\n", m, x);
                cpu->halt = true;
                return;
            }
            return;
        }
        case InstructionType::IntLd_StImm_5: {
            uint8_t b3532 = (slot >> 32) & 0xf;
            uint8_t x6 = (slot >> 30) & 3;
            if ((b3532 == 0) && (x6 == 3)) {
                DEFINEM3VARS;
                uint8_t size = 8;
                printf("(qp %d) ld8%s r%d = [r%d], %d\n", qp, Hint::GetHintStr(hint), r1, r3, imm9);
                if (cpu->branched) {
                    return;
                }
                if (cpu->regs.pr[qp].val) {
                    if (r1 == r3) {
                        cpu->IllegalOperationFault();
                    }
                    cpu->regs.CheckTargetRegister(r1);
                    cpu->regs.CheckTargetRegister(r3);
                    if (cpu->regs.gpr[r3].nat) {
                        cpu->RegisterNatConsumptionFault(0); // READ
                    }
                    //paddr = tlb_translate(GR[r3], size, itype, PSR.cpl, &mattr, &defer);
                    uint64_t paddr = cpu->regs.gpr[r3].val;
                    uint64_t val = 0;

                    Memory::ReadAt<uint64_t>(&val, paddr);
                    //printf("read %lx at %lx\n", val, paddr);
                    cpu->regs.gpr[r1] = ZeroExt(val, size * 8);
                    cpu->regs.gpr[r1] = false;
                    cpu->regs.gpr[r3] = cpu->regs.gpr[r3].val + SignExt(imm9, 9);
                    cpu->regs.gpr[r3].nat = cpu->regs.gpr[r3].nat; // yes this is in the manual
                    if (cpu->regs.gpr[r3].nat) {
                        // mem_implicit_prefetch(cpu->regs.gpr[r3], ldhint | bias, itype);
                    }
                }
            } else {
                fprintf(stderr, "unimpl m unit inst type 5 b3532 %x x6 %x\n", b3532, x6);
                cpu->halt = true;
                return;
            }
            return;
        }
        case InstructionType::ALU_MMALU_8: {
            ALU::MMALUHandle(bundle, cpu, slot);
            return;
        }
        case InstructionType::AddImm22_9: {
            AddImm22::Handle(bundle, cpu, slot);
            return;
        }
        case InstructionType::Compare_E: {
            Compare::Handle(bundle, cpu, slot);
            return;
        }
        default: {
            fprintf(stderr, "unimplemented  m unit instruction type %x!!\n", instType);
            cpu->halt = true;
            return;
        }
    }
}

}
