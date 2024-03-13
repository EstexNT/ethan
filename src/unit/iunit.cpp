#include "iunit.hpp"

namespace IUnit {

DECLINST(UnimplInst) {
    fprintf(stderr, "unimpl iunit op=%d\n", format->common.op);
    cpu->halt = true;
}
DECLINST(UnimplInstOp0) {
    fprintf(stderr, "unimpl iunit op=0 x3=%d\n", format->i19.x3);
    cpu->halt = true;
}
DECLINST(UnimplInstOp0X30) {
    fprintf(stderr, "unimpl iunit op=0 x3=0 30:27=%ld x6=%ld\n", (format->raw >> 27) & 0xf, (format->raw >> 31) & 3);
    cpu->halt = true;
}

// op = 0 //

DECLINST(NopI) {
    uint32_t imm = (format->i19.i << 20) | format->i19.imm20a;
    printf("(qp %d) nop.i 0x%08x\n", format->i19.qp, imm);
    if (cpu->regs.pr[format->i19.qp].val) {
        // no operation
    }
}

DECLINST(MiscExt) {
    // weird abuse of x6 here but ok
    static HandleFn miscexthandle[16][4] = {
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { NopI, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
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
    miscexthandle[(format->raw >> 27) & 0xf][(format->raw >> 31) & 3](format, cpu);
}

DECLINST(Misc) {
    static HandleFn op0handle[8] = {
        MiscExt, UnimplInstOp0, UnimplInstOp0, UnimplInstOp0,
        UnimplInstOp0, UnimplInstOp0, UnimplInstOp0, UnimplInstOp0,
    };
    op0handle[format->i19.x3](format, cpu);
}

// 4.3 page 282
//                  [op]
HandleFn handletable[8] = {
    Misc, UnimplInst, UnimplInst, UnimplInst,
    UnimplInst, UnimplInst, UnimplInst, UnimplInst,
};


} // IUnit
