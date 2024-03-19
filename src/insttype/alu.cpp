#include "alu.hpp"

namespace ALU {


DECLINST(UnimplInst) {
    fprintf(stderr, "unimpl alu op=%d\n", format->common.op);
    cpu->halt = true;
}

// op = 9 //
DECLINST(AddImm22) {
    uint32_t imm22 = SignExt(
        (format->a5.s << 21) | (format->a5.imm5c << 16) | (format->a5.imm9d << 7) | (format->a5.imm7b), 22);
    printf("(qp %d) addl r%d = 0x%08x, r%d\n", format->a5.qp, format->a5.r1, imm22, format->a5.r3);
    if (cpu->regs.pr[format->a5.qp].val) {
        cpu->regs.CheckTargetRegister(format->a5.r1);
        uint64_t tmpSrc = SignExt(imm22, 22);
        cpu->regs.gpr[format->a5.r1] = tmpSrc + cpu->regs.gpr[format->a5.r3].val;
        cpu->regs.gpr[format->a5.r1] = cpu->regs.gpr[format->a5.r3].nat;
    }
}


// 4.2 page 272
//                  [op]
HandleFn handletable[8] = {
    UnimplInst, AddImm22, UnimplInst, UnimplInst,
    UnimplInst, UnimplInst, UnimplInst, UnimplInst,
};

} // ALU
