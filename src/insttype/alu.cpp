#include "alu.hpp"

namespace ALU {


DECLINST(UnimplInst) {
    fprintf(stderr, "unimpl alu op=%d\n", format->common.op);
    cpu->halt = true;
}
DECLINST(UnimplInstCompare) {
    fprintf(stderr, "unimpl alu compare op=%d, x2=%d, tb=%d, ta=%d, c=%d\n", format->common.op, format->a6.x2, format->a6.tb, format->a6.ta, format->a6.c);
    cpu->halt = true;
}
DECLINST(UnimplInstALUMM) {
    fprintf(stderr, "unimpl alu alu/mm op=%d, ve=%d, x2a=%d\n", format->common.op, format->a1.ve, format->a1.x2a);
    cpu->halt = true;
}

// op = 8 //
DECLINST(AddsImm14) {
    uint32_t imm14 = SignExt((format->a4.s << 13) | (format->a4.imm6d << 7) | (format->a4.imm7b), 14);
    printf("(qp %d) add r%d = 0x%08x, r%d\n", format->a4.qp, format->a4.r1, imm14, format->a4.r3);
    if (cpu->regs.pr[format->a4.qp].val) {
        cpu->regs.CheckTargetRegister(format->a4.r1);
        uint64_t tmpSrc = SignExt(imm14, 14);
        cpu->regs.gpr[format->a4.r1] = tmpSrc + cpu->regs.gpr[format->a4.r3].val;
        cpu->regs.gpr[format->a4.r1] = cpu->regs.gpr[format->a4.r3].nat;
    }
}

DECLINST(ALUMMALU) {
    static HandleFn alummtable[2][4] = {
        {UnimplInstALUMM, UnimplInstALUMM, AddsImm14,       UnimplInstALUMM},
        {UnimplInstALUMM, UnimplInstALUMM, UnimplInstALUMM, UnimplInstALUMM},
        
    };
    alummtable[format->a1.ve][format->a1.x2a](format, cpu);
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


// op = 12 //
// op = 13 //
// op = 14 // 
// TODO: maybe handle everything in a single function

DECLINST(CmpLtImm8) {
    uint64_t imm = SignExt((format->a8.s << 7) | (format->a8.imm7b), 8);
    printf("(qp %d) cmp.lt p%d, p%d = %ld, r%d\n", format->a8.qp, format->a8.p1, format->a8.p2, imm, format->a8.r3);
    if (cpu->regs.pr[format->a8.qp].val) {
        if (format->a8.p1 == format->a8.p2) {
            cpu->IllegalOperationFault();
        }
        bool tmp_rel = (int64_t)(imm) < (int64_t)(cpu->regs.gpr[format->a8.r3].val);
        if (cpu->regs.gpr[format->a8.r3].nat) {
            cpu->regs.pr[format->a8.p1] = 0;
            cpu->regs.pr[format->a8.p2] = 0;
        } else {
            cpu->regs.pr[format->a8.p1] = tmp_rel;
            cpu->regs.pr[format->a8.p2] = !tmp_rel;
        }
    }
}

DECLINST(CmpEqImm8) {
    uint64_t imm = SignExt((format->a8.s << 7) | (format->a8.imm7b), 8);
    printf("(qp %d) cmp.eq p%d, p%d = %ld, r%d\n", format->a8.qp, format->a8.p1, format->a8.p2, imm, format->a8.r3);
    if (cpu->regs.pr[format->a8.qp].val) {
        if (format->a8.p1 == format->a8.p2) {
            cpu->IllegalOperationFault();
        }
        bool tmp_rel = (imm == cpu->regs.gpr[format->a8.r3].val);
        if (cpu->regs.gpr[format->a8.r3].nat) {
            cpu->regs.pr[format->a8.p1] = 0;
            cpu->regs.pr[format->a8.p2] = 0;
        } else {
            cpu->regs.pr[format->a8.p1] = tmp_rel;
            cpu->regs.pr[format->a8.p2] = !tmp_rel;
        }
    }
}

DECLINST(CmpEqUncImm8) {
    uint64_t imm = SignExt((format->a8.s << 7) | (format->a8.imm7b), 8);
    printf("(qp %d) cmp.eq.unc p%d, p%d = %ld, r%d\n", format->a8.qp, format->a8.p1, format->a8.p2, imm, format->a8.r3);
    if (cpu->regs.pr[format->a8.qp].val) {
        if (format->a8.p1 == format->a8.p2) {
            cpu->IllegalOperationFault();
        }
        bool tmp_rel = (imm == cpu->regs.gpr[format->a8.r3].val);
        if (cpu->regs.gpr[format->a8.r3].nat) {
            cpu->regs.pr[format->a8.p1] = 0;
            cpu->regs.pr[format->a8.p2] = 0;
        } else {
            cpu->regs.pr[format->a8.p1] = tmp_rel;
            cpu->regs.pr[format->a8.p2] = !tmp_rel;
        }
    } else {
        if (format->a8.p1 == format->a8.p2) {
            cpu->IllegalOperationFault();
        }
        cpu->regs.pr[format->a8.p1] = 0;
        cpu->regs.pr[format->a8.p2] = 0;
    }
}
DECLINST(CmpEq) {
    printf("(qp %d) cmp.eq p%d, p%d = r%d, r%d\n", format->a6.qp, format->a6.p1, format->a6.p2, format->a6.r2, format->a6.r3);
    if (cpu->regs.pr[format->a6.qp].val) {
        if (format->a6.p1 == format->a6.p2) {
            cpu->IllegalOperationFault();
        }
        bool tmpRel = (cpu->regs.gpr[format->a6.r2].val == cpu->regs.gpr[format->a6.r3].val);
        if (cpu->regs.gpr[format->a6.r3].nat || cpu->regs.gpr[format->a6.r2].nat ) {
            cpu->regs.pr[format->a6.p1] = 0;
            cpu->regs.pr[format->a6.p2] = 0;
        } else {
            cpu->regs.pr[format->a6.p1] = tmpRel;
            cpu->regs.pr[format->a6.p2] = !tmpRel;
        }
    }
}

DECLINST(Compare) {
    //                          [op][x2][tb][ta][c]
    static HandleFn comparetable[3][4][2][2][2] = {
        { // [0][0]xxx
        { { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
          { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
        }, // [0][1]xxx
        { { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
          { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
        }, // [0][2]xxx
        { { {CmpLtImm8,         UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
          { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
        }, // [0][3]xxx
        { { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
          { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
        },
        },
        { // [1][0]xxx
        { { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
          { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
        }, // [1][1]xxx
        { { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
          { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
        }, // [1][2]xxx
        { { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
          { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
        }, // [1][3]xxx
        { { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
          { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
        },
        },
        { // [2][0]xxx
        { { {CmpEq,             UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
          { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
        }, // [2][1]xxx
        { { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
          { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
        }, // [2][2]xxx
        { { {CmpEqImm8,         CmpEqUncImm8     }, {UnimplInstCompare, UnimplInstCompare}, }, 
          { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
        }, // [2][3]xxx
        { { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
          { {UnimplInstCompare, UnimplInstCompare}, {UnimplInstCompare, UnimplInstCompare}, }, 
        },
        },
    };
    comparetable[format->common.op - 0xc][format->a6.x2][format->a6.tb][format->a6.ta][format->a6.c](format, cpu);
}

// 4.2 page 272
//                  [op]
HandleFn handletable[8] = {
    ALUMMALU,   AddImm22,   UnimplInst, UnimplInst,
    Compare,    Compare,    Compare,    UnimplInst,
};

} // ALU
