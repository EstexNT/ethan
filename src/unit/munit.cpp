#include "munit.hpp"

namespace MUnit {

static inline void CheckTargetRegisterSof(uint64_t r1, uint64_t newsof) {
    debugprintf("TODO: check_target_register_sof(%lx, %lx)\n", r1, newsof);
    // If r1 targets an out-of-frame stacked register (as defined by the newsof parameter), 
    // an illegal operation fault is delivered, and this function does not return.
    // what does ^^^ mean??
}

DECLINST(UnimplInst) {
    fprintf(stderr, "unimpl memory op=%d\n", format->common.op);
    cpu->halt = true;
}
DECLINST(UnimplInstOp0) {
    fprintf(stderr, "unimpl memory op=0 x3=%d\n", format->m37.x3);
    cpu->halt = true;
}
DECLINST(UnimplInstOp0X30) {
    fprintf(stderr, "unimpl memory op=0 x3=0 x4=%d x2=%d\n", format->m37.x4, format->m37.x2);
    cpu->halt = true;
}


// op = 0 //

DECLINST(NopM) {
    uint32_t imm = (format->m37.i << 20) | format->m37.imm20a;
    printf("(qp %d) nop.m 0x%08x\n", format->m37.qp, imm);
    if (cpu->regs.pr[format->m37.qp].val) {
        // no operation
    }
}

DECLINST(SysMemMgmtExt) {
    //                         [x4][x2]
    static HandleFn sysexttable[16][4] = {
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { NopM, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
    };
    sysexttable[format->m37.x4][format->m37.x2](format, cpu);
}

DECLINST(SysMemMgmt0) {
    static HandleFn op0handle[8] = {
        SysMemMgmtExt, UnimplInstOp0, UnimplInstOp0, UnimplInstOp0,
        UnimplInstOp0, UnimplInstOp0, UnimplInstOp0, UnimplInstOp0,
    };
    op0handle[format->m37.x3](format, cpu);
}


// 4.4 page 296
//                  [op]
HandleFn handletable[8] = {
    SysMemMgmt0, UnimplInst, UnimplInst, UnimplInst, 
    UnimplInst, UnimplInst, UnimplInst, UnimplInst, 
};


} // MUnit
