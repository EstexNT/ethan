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

DECLINST(UnimplInst) {
    fprintf(stderr, "unimpl branch op=%d\n", format->common.op);
    cpu->halt = true;
}
DECLINST(UnimplInstOp4) {
    fprintf(stderr, "unimpl branch op=4 btype=%d\n", format->b1.btype);
    cpu->halt = true;
}
DECLINST(UnimplInstMisc) {
    fprintf(stderr, "unimpl branch misc b30:27=%d x6=%d\n", format->b8.x6 & 0b1111, (format->b8.x6 & 0b110000) >> 4);
    cpu->halt = true;
}


// op = 4 //

DECLINST(BrCond) {
    uint64_t tempIP = AlignIP(cpu->regs.ip + SignExt(((format->b1.s << 20) | format->b1.imm20b) << 4, 25));
    
    printf("(qp %d) br.cond%s%s%s 0x%lx\n", format->b1.qp, 
        GetBranchWhetherHintCompleterStr(format->b1.wh),
        GetSequentialPrefetchHintCompleterStr(format->b1.p),
        GetBranchCacheDeallocHintCompleterStr(format->b1.d),
        tempIP
        );
    if (cpu->regs.pr[format->b1.qp].val) {
        cpu->branched = true;
        cpu->regs.ip = tempIP;
        if ((cpu->regs.psr.it && cpu->UnimplementedVirtualAddress(tempIP)) ||
            (!cpu->regs.psr.it && cpu->UnimplementedVirtualAddress(tempIP))) {
            cpu->UnimplementedInstructionAddressTrap(0, tempIP);
        }
        if (cpu->regs.psr.tb) {
            cpu->TakenBranchTrap();
        }
    }
}
DECLINST(BrCloop) {
    uint64_t tempIP = AlignIP(cpu->regs.ip + SignExt(((format->b2.s << 20) | format->b2.imm20b) << 4, 25));
    
    printf("(qp %d) br.cloop%s%s%s 0x%lx\n", format->b2.qp, 
        GetBranchWhetherHintCompleterStr(format->b2.wh),
        GetSequentialPrefetchHintCompleterStr(format->b2.p),
        GetBranchCacheDeallocHintCompleterStr(format->b2.d),
        tempIP
        );
    cpu->regs.ar[Ia64Regs::Ar::LC].val--;
    if (cpu->regs.ar[Ia64Regs::Ar::LC].val != 0) {
        cpu->branched = true;
        cpu->regs.ip = tempIP;
        if ((cpu->regs.psr.it && cpu->UnimplementedVirtualAddress(tempIP)) ||
            (!cpu->regs.psr.it && cpu->UnimplementedVirtualAddress(tempIP))) {
            cpu->UnimplementedInstructionAddressTrap(0, tempIP);
        }
        if (cpu->regs.psr.tb) {
            cpu->TakenBranchTrap();
        }

    }
}

DECLINST(IPRelBrType) {
    static HandleFn op4handle[8] = {
        BrCond,        UnimplInstOp4, UnimplInstOp4, UnimplInstOp4,
        UnimplInstOp4, BrCloop,       UnimplInstOp4, UnimplInstOp4,
    };

    op4handle[format->b1.btype](format, cpu);
}


// op = 0 //
DECLINST(Rfi) {
    printf("rfi\n");
    uint64_t tmpIP = 0;
    // if (!FollowedByStop()) {
    //     cpu->IllegalOperationFault();  
    // }
    uint64_t unimplementedAddress = 0;
    if (cpu->regs.psr.cpl != 0) {
        cpu->PrivilegedOperationFault(0);
    }
    uint64_t takenRfi = 1;
    cpu->regs.psr.raw = cpu->regs.cr[Ia64Regs::Cr::Type::IPSR].val;
    if (cpu->regs.psr.is == 1) {
        fprintf(stderr, "\n\nresume ia-32 instruction set\n\n");
        cpu->halt = true;
        return;
    } else {
        tmpIP = AlignIP(cpu->regs.cr[Ia64Regs::Cr::Type::IIP].val);
        uint64_t slot = cpu->regs.psr.ri;
        if ((cpu->regs.psr.it && cpu->UnimplementedVirtualAddress(tmpIP)) ||
           (!cpu->regs.psr.it && cpu->UnimplementedPhysicalAddress(tmpIP))) {
            unimplementedAddress = 1;
        }
        // cr[IFS].v
        if ((cpu->regs.cr[Ia64Regs::Cr::Type::IFS].val >> 63) & 1) {
            uint32_t tmpGrowth = -cpu->regs.cfm.sof;
            cpu->alat.FrameUpdate(-(cpu->regs.cr[Ia64Regs::Cr::Type::IFS].val & 0b1111111), 0);
            cpu->rse.RestoreFrame(cpu->regs.cr[Ia64Regs::Cr::Type::IFS].val & 0b1111111, tmpGrowth, cpu->regs.cfm.sof);
            cpu->regs.cfm.raw = (cpu->regs.cr[Ia64Regs::Cr::Type::IFS].val & 0x1fffffffff);
        }
        cpu->rse.EnableCurrentFrameLoad();
    }
    cpu->regs.ip = tmpIP;
    cpu->branched = true;
    // cpu->InstructionSerialize();
    if (unimplementedAddress) {
        cpu->UnimplementedInstructionAddressTrap(0, tmpIP);
    }
}

DECLINST(MiscIndirectBranch) {
    static HandleFn miscbranchtable[16][4] = {
        {UnimplInstMisc, UnimplInstMisc, UnimplInstMisc, UnimplInstMisc},
        {UnimplInstMisc, UnimplInstMisc, UnimplInstMisc, UnimplInstMisc},
        {UnimplInstMisc, UnimplInstMisc, UnimplInstMisc, UnimplInstMisc},
        {UnimplInstMisc, UnimplInstMisc, UnimplInstMisc, UnimplInstMisc},
        {UnimplInstMisc, UnimplInstMisc, UnimplInstMisc, UnimplInstMisc},
        {UnimplInstMisc, UnimplInstMisc, UnimplInstMisc, UnimplInstMisc},
        {UnimplInstMisc, UnimplInstMisc, UnimplInstMisc, UnimplInstMisc},
        {UnimplInstMisc, UnimplInstMisc, UnimplInstMisc, UnimplInstMisc},
        {Rfi,            UnimplInstMisc, UnimplInstMisc, UnimplInstMisc},
        {UnimplInstMisc, UnimplInstMisc, UnimplInstMisc, UnimplInstMisc},
        {UnimplInstMisc, UnimplInstMisc, UnimplInstMisc, UnimplInstMisc},
        {UnimplInstMisc, UnimplInstMisc, UnimplInstMisc, UnimplInstMisc},
        {UnimplInstMisc, UnimplInstMisc, UnimplInstMisc, UnimplInstMisc},
        {UnimplInstMisc, UnimplInstMisc, UnimplInstMisc, UnimplInstMisc},
        {UnimplInstMisc, UnimplInstMisc, UnimplInstMisc, UnimplInstMisc},
        {UnimplInstMisc, UnimplInstMisc, UnimplInstMisc, UnimplInstMisc},
    };

    miscbranchtable[format->b8.x6 & 0b1111][(format->b8.x6 & 0b110000) >> 4](format, cpu);
}



// 4.5 page 324
//                  [op]
HandleFn handletable[8] = {
    MiscIndirectBranch,  UnimplInst, UnimplInst, UnimplInst, 
    IPRelBrType,         UnimplInst, UnimplInst, UnimplInst,
};


} // BUnit
