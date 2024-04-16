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

DECLINST(IPRelBrType) {
    static HandleFn op4handle[8] = {
        BrCond,        UnimplInstOp4, UnimplInstOp4, UnimplInstOp4,
        UnimplInstOp4, UnimplInstOp4, UnimplInstOp4, UnimplInstOp4,
    };

    op4handle[format->b1.btype](format, cpu);
}

// 4.5 page 324
//                  [op]
HandleFn handletable[8] = {
    UnimplInst,  UnimplInst, UnimplInst, UnimplInst, 
    IPRelBrType, UnimplInst, UnimplInst, UnimplInst,
};


} // BUnit
