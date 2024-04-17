#include "lxunit.hpp"

namespace LXUnit {

DECLINSTLX(UnimplInst) {
    fprintf(stderr, "unimpl l+x op=%d\n", format1->common.op);
    cpu->halt = true;
}


// op = 6 //
DECLINSTLX(Movl) {
    uint64_t imm64 = ((uint64_t)format1->x2.i << 63) | ((format0->raw & 0x1ffffffffff) << 22) | (format1->x2.ic << 21) 
                    | (format1->x2.imm5c << 16) | (format1->x2.imm9d << 7) | (format1->x2.imm7b);
    printf("(qp %d) movl r%d = 0x%lx\n", format1->x2.qp, format1->x2.r1, imm64);
    if (cpu->regs.pr[format1->x2.qp].val) {
        cpu->regs.CheckTargetRegister(format1->x2.r1);

        cpu->regs.gpr[format1->x2.r1] = imm64;
        cpu->regs.gpr[format1->x2.r1] = false;
    }
}


LXHandleFn handletable[16] = {
    UnimplInst, UnimplInst, UnimplInst, UnimplInst, 
    UnimplInst, UnimplInst, Movl,       UnimplInst, 
    UnimplInst, UnimplInst, UnimplInst, UnimplInst, 
    UnimplInst, UnimplInst, UnimplInst, UnimplInst, 
};


} // LXUnit
