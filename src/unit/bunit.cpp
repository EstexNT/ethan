#include "bunit.hpp"

namespace BUnit {

static inline const char *GetBranchWhetherHintCompleterStr(uint8_t v) {
    if (v > 3) {
        return "";
    }
    return (const char *[]){".sptk", ".spnt", ".dptk", ".dpnt"}[v];
}
static inline const char *GetSequentialPrefetchHintCompleterStr(uint8_t v) {
    if (v > 1) {
        return "";
    }
    return (const char *[]){".few", ".many"}[v];
}
static inline const char *GetBranchCacheDeallocHintCompleterStr(uint8_t v) {
    if (v > 1) {
        return "";
    }
    return (const char *[]){"", ".clr"}[v];
}


void Handle(Ia64Bundle *bundle, Ia64Cpu *cpu, uint64_t slot) {
    uint8_t instType = (slot >> 37) & 0xf;
    switch (instType) {
        case InstructionType::IndirectPredict_Nop_2: {
            uint8_t x6 = (slot >> 31) & 3;
            uint8_t b3027 = (slot >> 27) & 0xf;
            
            if ((b3027 == 0) && (x6 == 0)) {
                uint8_t qp = (slot >> 0) & 63;
                uint32_t imm20b = (slot >> 6) & 0xfffff;
                uint8_t i = (slot >> 36) & 1;
                uint32_t imm21 = (i << 20) | (imm20b);

                printf("(qp %d) nop.b %08x\n", qp, imm21);
                if (cpu->regs.pr[qp].val) {
                    // no operation
                }
                return;
            }
            return;
        }
        case InstructionType::IPRelativeBranch_4: {
            uint8_t qp = (slot >> 0) & 63;
            uint8_t btype = (slot >> 6) & 7;
            uint8_t p = (slot >> 12) & 1;
            uint32_t imm20b = (slot >> 13) & 0xfffff;
            uint8_t wh = (slot >> 33) & 3;
            uint8_t d = (slot >> 35) & 1;
            uint8_t s = (slot >> 36) & 1;
            uint32_t imm = (s << 20) | imm20b;
            uint64_t tmp_IP = AlignIP(cpu->regs.ip + SignExt(imm << 4, 25));

            switch (btype) {
                case 0: {
                    printf("(qp %d) br.cond%s%s%s 0x%lx\n", qp, 
                    GetBranchWhetherHintCompleterStr(wh),
                    GetSequentialPrefetchHintCompleterStr(p),
                    GetBranchCacheDeallocHintCompleterStr(d),
                    tmp_IP
                    );
                    if (cpu->branched) {
                        return;
                    }
                    bool tmp_taken = cpu->regs.pr[qp].val;
                    if (tmp_taken) {
                        cpu->branched = true;
                        cpu->regs.ip = tmp_IP;
                        if ((cpu->regs.psr.it && cpu->UnimplementedVirtualAddress(tmp_IP)) ||
                            (!cpu->regs.psr.it && cpu->UnimplementedVirtualAddress(tmp_IP))) {
                                cpu->UnimplementedInstructionAddressTrap(0, tmp_IP);
                            }
                        if (cpu->regs.psr.tb) {
                            cpu->TakenBranchTrap();
                        }
                    }
                    return;
                }
                default: {
                    fprintf(stderr, "unimpl b unit insn btype %x\n", btype);
                    cpu->halt = true;
                    return;
                }
            }

            return;
        }
        case InstructionType::IPRelativeCall_5: {
            uint8_t qp = (slot >> 0) & 63;
            uint8_t b1 = (slot >> 6) & 7;
            uint8_t p = (slot >> 12) & 1;
            uint32_t imm20b = (slot >> 13) & 0xfffff;
            uint8_t wh = (slot >> 33) & 3;
            uint8_t d = (slot >> 34) & 1;
            uint8_t s = (slot >> 36) & 1;
            uint32_t imm = (s << 20) | imm20b;


            // 1002340
            //printf("%x\n", s);
            //printf("%x\n", imm);
            //printf("%lx\n", cpu->regs.ip);
            uint64_t tmp_IP = AlignIP(cpu->regs.ip + SignExt(imm << 4, 25));
            //printf("%lx\n", tmp_IP);

            printf("(qp %d) br.call%s%s%s b%d = 0x%lx\n", qp,
                    GetBranchWhetherHintCompleterStr(wh),
                    GetSequentialPrefetchHintCompleterStr(p),
                    GetBranchCacheDeallocHintCompleterStr(d),
                    b1, tmp_IP);
            if (cpu->branched) {
                return;
            }
            bool tmp_taken = cpu->regs.pr[qp].val;
            if (tmp_taken) {
                cpu->regs.br[b1] = cpu->regs.ip + IA64BUNDLESIZE;
                
                cpu->regs.ar[Ia64Regs::Ar::Type::PFS].SetPFS(cpu->regs.cfm.raw, 
                                                cpu->regs.ar[Ia64Regs::Ar::Type::EC].val, cpu->regs.psr.cpl);
                cpu->alat.FrameUpdate(cpu->regs.cfm.sol, 0);
                cpu->rse.PreserveFrame(cpu->regs.cfm.sol);
                cpu->regs.cfm.sof -= cpu->regs.cfm.sol;
                cpu->regs.cfm.sol = 0;
                cpu->regs.cfm.sor = 0;
                cpu->regs.cfm.rrb.gr = 0;
                cpu->regs.cfm.rrb.fr = 0;
                cpu->regs.cfm.rrb.pr = 0;
            }
            if (tmp_taken) {
                cpu->branched = true;
                cpu->regs.ip = tmp_IP;
                if ((cpu->regs.psr.it && cpu->UnimplementedVirtualAddress(tmp_IP)) ||
                    (!cpu->regs.psr.it && cpu->UnimplementedVirtualAddress(tmp_IP))) {
                        cpu->UnimplementedInstructionAddressTrap(0, tmp_IP);
                    }
                if (cpu->regs.psr.tb) {
                    cpu->TakenBranchTrap();
                }
            }
            return;
        }
        default: {
            fprintf(stderr, "unimpl b unit inst type %x!!\n", instType);
            cpu->halt = true;
            return;
        }
    }
}

} // BUnit
