#include "funit.hpp"

namespace FUnit {


DECLINST(UnimplInst) {
    fprintf(stderr, "unimpl funit op=%d\n", format->common.op);
    cpu->halt = true;
}
DECLINST(UnimplInstOp) {
    fprintf(stderr, "unimpl funit inst op=%d x=%ld\n", format->common.op, (format->raw >> 33) & 1);
    cpu->halt = true;
}
DECLINST(UnimplInstOpX0) {
    fprintf(stderr, "unimpl funit inst op=%d x=0 b30:27=%ld b32:31=%ld\n", format->common.op, (format->raw >> 27) & 15, (format->raw >> 31) & 3);
    cpu->halt = true;
}


// op = 0 //
DECLINST(NopF) {
    uint32_t imm = (format->f15.i << 20) | format->f15.imm20a;
    printf("(qp %d) nop.f 0x%08x\n", format->f15.qp, imm);
    if (ISQP(f15)) {
        // no operation
    }
}

DECLINST(FPMiscOpX0) {
    static HandleFn fpmiscopx0table[2][16][4] = {
        { 
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {NopF,           UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
        },
        { 
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
            {UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0, UnimplInstOpX0},
        },
    };
    fpmiscopx0table[format->common.op][(format->raw >> 27) & 15][(format->raw >> 31) & 3](format, cpu);
}

DECLINST(FPMisc) {
    static HandleFn fpmisctable[2][2] = {
        {FPMiscOpX0, UnimplInstOp},
        {FPMiscOpX0, UnimplInstOp},
    };
    fpmisctable[format->common.op][(format->raw >> 33) & 1](format, cpu);
}

// 4.6 page 331
HandleFn handletable[0x10] = {
    FPMisc,     FPMisc,     UnimplInst, UnimplInst,
    UnimplInst, UnimplInst, UnimplInst, UnimplInst,
    UnimplInst, UnimplInst, UnimplInst, UnimplInst,
    UnimplInst, UnimplInst, UnimplInst, UnimplInst,
};

}