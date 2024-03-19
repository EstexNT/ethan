#include "iunit.hpp"
#include "hint.hpp"

namespace IUnit {

static inline const char *GetMoveToBRWhetherHintStr(uint8_t mwh) {
    if (mwh > 3) {
        return "";
    }
    return (const char *[]){".sptk", "", ".dptk", ""}[mwh];
}

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


DECLINST(MovFromIP) {
    printf("(qp %d) mov r%d = ip\n", format->i25.qp, format->i25.r1);
    if (cpu->regs.pr[format->i25.qp].val) {
        cpu->regs.CheckTargetRegister(format->i25.r1);
        cpu->regs.gpr[format->i25.r1] = cpu->regs.ip;
        cpu->regs.gpr[format->i25.r1] = false;
    }
}
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
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, MovFromIP },
        { NopI,             UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
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

DECLINST(MovToB) {
    printf("(qp %d) mov%s%s%s b%d = r%d, %x\n", format->i21.qp, (format->i21.x) ? ".ret" : "",
    GetMoveToBRWhetherHintStr(format->i21.wh), Hint::GetBranchImportanceHintStr(format->i21.ih), format->i21.b1, format->i21.r2, format->i21.timm9c);
    if (cpu->regs.pr[format->i21.qp].val) {
        uint64_t tmp_tag = cpu->regs.ip + SignExt(format->i21.timm9c << 4, 13);
        if (cpu->regs.gpr[format->i21.r2].nat) {
            cpu->RegisterNatConsumptionFault(0);
        }
        cpu->regs.br[format->i21.b1] = cpu->regs.gpr[format->i21.r2].val;
        // TODO: branch_predict(mwh, ih, return_form, gr[r2], tmp_tag);
    }
}

DECLINST(Misc) {
    static HandleFn op0handle[8] = {
        MiscExt, UnimplInstOp0, UnimplInstOp0, UnimplInstOp0,
        UnimplInstOp0, UnimplInstOp0, UnimplInstOp0, MovToB,
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
