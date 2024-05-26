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
    if (ISQP(a4)) {
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
    if (ISQP(a5)) {
        cpu->regs.CheckTargetRegister(format->a5.r1);
        uint64_t tmpSrc = SignExt(imm22, 22);
        cpu->regs.gpr[format->a5.r1] = tmpSrc + cpu->regs.gpr[format->a5.r3].val;
        cpu->regs.gpr[format->a5.r1] = cpu->regs.gpr[format->a5.r3].nat;
    }
}


// op = 12 //
// op = 13 //
// op = 14 // 
enum Form {
    RegisterForm = 0,
    Imm8Form = 1,
    ParallelInequalityForm = 2,
};
enum CType {
    None = 0,
    Unc = 1,
    Or = 2,
    And = 3,
    OrAndcm = 4,
};
enum CRel {
    Eq = 0,
    Ne = 1,
    Lt = 2,
    Le = 3,
    Gt = 4,
    Ge = 5,
    Ltu = 6,
    Leu = 7,
    Gtu = 8,
    Geu = 9,
};
static inline const char *GetCRelString(uint8_t crel) {
    if (crel > 9) {
        return "";
    }
    return (const char *[]){".eq", ".ne", ".lt", ".le", ".gt", ".ge", ".ltu", ".leu", ".gtu", ".geu"}[crel];
}
static inline const char *GetCTypeString(uint8_t ctype) {
    if (ctype > 4) {
        return "";
    }
    return (const char *[]){"", ".unc", ".or", ".and", ".or.andcm"}[ctype];
}

DECLINST(Compare) {
    static const struct {
        Form fm;
        CRel rl;
        CType tp;
    //           [op][x2][tb][ta][c]
    } comparetable[3][4][2][2][2] = {
        { // [0][0]xxx
        { { {{RegisterForm, Lt, None}, {RegisterForm, Lt, Unc}}, 
            {{RegisterForm, Eq, And},  {RegisterForm, Ne, And}}, 
          }, 
          { {{RegisterForm, Gt, And},  {RegisterForm, Le, And}}, 
            {{RegisterForm, Ge, And},  {RegisterForm, Lt, And}}, 
          }, 
        }, // [0][1]xxx cmp4
        { { {{RegisterForm, Lt, None}, {RegisterForm, Lt, Unc}}, 
            {{RegisterForm, Eq, And},  {RegisterForm, Ne, And}}, 
          }, 
          { {{RegisterForm, Gt, And},  {RegisterForm, Le, And}}, 
            {{RegisterForm, Ge, And},  {RegisterForm, Lt, And}}, 
          }, 
        }, // [0][2]xxx imm8 (tb is the sign in a8 so it's repeated)
        { { {{Imm8Form, Lt, None},     {Imm8Form, Lt, Unc}},
            {{Imm8Form, Eq, And},      {Imm8Form, Ne, And}},
          }, 
          { {{Imm8Form, Lt, None},     {Imm8Form, Lt, Unc}},
            {{Imm8Form, Eq, And},      {Imm8Form, Ne, And}},
          }, 
        }, // [0][3]xxx cmp4 imm8
        { { {{Imm8Form, Lt, None},     {Imm8Form, Lt, Unc}},
            {{Imm8Form, Eq, And},      {Imm8Form, Ne, And}},
          }, 
          { {{Imm8Form, Lt, None},     {Imm8Form, Lt, Unc}},
            {{Imm8Form, Eq, And},      {Imm8Form, Ne, And}},
          }, 
        },
        },
        { // [1][0]xxx
        { { {{RegisterForm, Ltu, None}, {RegisterForm, Ltu, Unc}}, 
            {{RegisterForm, Eq, Or},    {RegisterForm, Ne, Or}  }, 
          }, 
          { {{RegisterForm, Gt, Or},    {RegisterForm, Le, Or}  }, 
            {{RegisterForm, Ge, Or},    {RegisterForm, Lt, Or}  }, 
          }, 
        }, // [1][1]xxx cmp4
        { { {{RegisterForm, Ltu, None}, {RegisterForm, Ltu, Unc}}, 
            {{RegisterForm, Eq, Or},    {RegisterForm, Ne, Or}  }, 
          }, 
          { {{RegisterForm, Gt, Or},    {RegisterForm, Le, Or}  }, 
            {{RegisterForm, Ge, Or},    {RegisterForm, Lt, Or}  }, 
          }, 
        }, // [1][2]xxx imm8
        { { {{Imm8Form, Ltu, None},     {Imm8Form, Ltu, Unc}}, 
            {{Imm8Form, Eq, Or},        {Imm8Form, Ne, Or}  }, 
          }, 
          { {{Imm8Form, Ltu, None},     {Imm8Form, Ltu, Unc}}, 
            {{Imm8Form, Eq, Or},        {Imm8Form, Ne, Or}  }, 
          }, 
        }, // [1][3]xxx cmp4 imm8
        { { {{Imm8Form, Ltu, None},     {Imm8Form, Ltu, Unc}}, 
            {{Imm8Form, Eq, Or},        {Imm8Form, Ne, Or}  }, 
          }, 
          { {{Imm8Form, Ltu, None},     {Imm8Form, Ltu, Unc}}, 
            {{Imm8Form, Eq, Or},        {Imm8Form, Ne, Or}  }, 
          }, 
        },
        },
        { // [2][0]xxx
        { { {{RegisterForm, Eq, None},    {RegisterForm, Eq, Unc}    }, 
            {{RegisterForm, Eq, OrAndcm}, {RegisterForm, Ne, OrAndcm}}, 
          }, 
          { {{RegisterForm, Gt, OrAndcm}, {RegisterForm, Le, OrAndcm}}, 
            {{RegisterForm, Ge, OrAndcm}, {RegisterForm, Lt, OrAndcm}}, 
          }, 
        }, // [2][1]xxx cmp4
        { { {{RegisterForm, Eq, None},    {RegisterForm, Eq, Unc}    }, 
            {{RegisterForm, Eq, OrAndcm}, {RegisterForm, Ne, OrAndcm}}, 
          }, 
          { {{RegisterForm, Gt, OrAndcm}, {RegisterForm, Le, OrAndcm}}, 
            {{RegisterForm, Ge, OrAndcm}, {RegisterForm, Lt, OrAndcm}}, 
          }, 
        }, // [2][2]xxx imm8
        { { {{Imm8Form, Eq, None},        {Imm8Form, Eq, Unc}    }, 
            {{Imm8Form, Eq, OrAndcm},     {Imm8Form, Ne, OrAndcm}}, 
          }, 
          { {{Imm8Form, Eq, None},        {Imm8Form, Eq, Unc}    }, 
            {{Imm8Form, Eq, OrAndcm},     {Imm8Form, Ne, OrAndcm}}, 
          }, 
        }, // [2][3]xxx cmp4 imm8
        { { {{Imm8Form, Eq, None},        {Imm8Form, Eq, Unc}    }, 
            {{Imm8Form, Eq, OrAndcm},     {Imm8Form, Ne, OrAndcm}}, 
          }, 
          { {{Imm8Form, Eq, None},        {Imm8Form, Eq, Unc}    }, 
            {{Imm8Form, Eq, OrAndcm},     {Imm8Form, Ne, OrAndcm}}, 
          },
        },
        },
    };
    CType ctype = comparetable[format->common.op - 0xc][format->a6.x2][format->a6.tb][format->a6.ta][format->a6.c].tp;
    CRel crel = comparetable[format->common.op - 0xc][format->a6.x2][format->a6.tb][format->a6.ta][format->a6.c].rl;
    Form form = comparetable[format->common.op - 0xc][format->a6.x2][format->a6.tb][format->a6.ta][format->a6.c].fm;

    if (format->a6.x2 & 1) { // 1 or 3
        printf("cmp4 not implemented\n");
        cpu->halt = true;
        return;
    }
    // 0 or 2
    if (ISQP(a6)) {
        uint8_t imm8 = SignExt((format->a8.s << 7) | (format->a8.imm7b), 8);
        switch (form) {
            case RegisterForm:
                printf("(qp %d) cmp%s%s p%d, p%d = r%d, r%d\n", format->a6.qp, GetCRelString(crel), GetCTypeString(ctype), 
                format->a6.p1, format->a6.p2, format->a6.r2, format->a6.r3);
                break;
            case Imm8Form:
                printf("(qp %d) cmp%s%s p%d, p%d = %d, r%d\n", format->a8.qp, GetCRelString(crel), GetCTypeString(ctype), 
                format->a8.p1, format->a8.p2, (int8_t)imm8, format->a8.r3);
                break;
            case ParallelInequalityForm:
                printf("(qp %d) cmp%s%s p%d, p%d = r0, r%d\n", format->a7.qp, GetCRelString(crel), GetCTypeString(ctype), 
                format->a7.p1, format->a7.p2, format->a7.r3);
                break;
        }
        if (format->a6.p1 == format->a6.p2) {
            cpu->IllegalOperationFault();
        }
        bool tmpNat = (form == RegisterForm ? cpu->regs.gpr[format->a6.r2].nat : 0) || cpu->regs.gpr[format->a6.r3].nat;
        uint64_t tmpSrc = 0;
        switch (form) {
            case RegisterForm:
                tmpSrc = cpu->regs.gpr[format->a6.r2].val;
                break;
            case Imm8Form:
                tmpSrc = SignExt(imm8, 8);
                break;
            case ParallelInequalityForm:
                break;
        }
        bool tmpRel = false;
        switch (crel) {
            case Eq:
                tmpRel = tmpSrc == cpu->regs.gpr[format->a6.r3].val;
                break;
            case Ne:
                tmpRel = tmpSrc != cpu->regs.gpr[format->a6.r3].val;
                break;
            case Lt:
                tmpRel = (int64_t)(tmpSrc) < (int64_t)(cpu->regs.gpr[format->a6.r3].val);
                break;
            case Le:
                tmpRel = (int64_t)(tmpSrc) <= (int64_t)(cpu->regs.gpr[format->a6.r3].val);
                break;
            case Gt:
                tmpRel = (int64_t)(tmpSrc) > (int64_t)(cpu->regs.gpr[format->a6.r3].val);
                break;
            case Ge:
                tmpRel = (int64_t)(tmpSrc) >= (int64_t)(cpu->regs.gpr[format->a6.r3].val);
                break;
            case Ltu:
                tmpRel = tmpSrc < cpu->regs.gpr[format->a6.r3].val;
                break;
            case Leu:
                tmpRel = tmpSrc <= cpu->regs.gpr[format->a6.r3].val;
                break;
            case Gtu:
                tmpRel = tmpSrc > cpu->regs.gpr[format->a6.r3].val;
                break;
            case Geu:
                tmpRel = tmpSrc >= cpu->regs.gpr[format->a6.r3].val;
                break;
        }
        switch (ctype) {
            case And:
                if (tmpNat || !tmpRel) {
                    cpu->regs.pr[format->a6.p1] = 0;
                    cpu->regs.pr[format->a6.p2] = 0;
                }
                break;
            case Or:
                if (!tmpNat && tmpRel) {
                    cpu->regs.pr[format->a6.p1] = 1;
                    cpu->regs.pr[format->a6.p2] = 1;
                }
                break;
            case OrAndcm:
                if (!tmpNat && tmpRel) {
                    cpu->regs.pr[format->a6.p1] = 1;
                    cpu->regs.pr[format->a6.p2] = 0;
                }
                break;
            case Unc:
            default:
                if (tmpNat) {
                    cpu->regs.pr[format->a6.p1] = 0;
                    cpu->regs.pr[format->a6.p2] = 0;
                } else {
                    cpu->regs.pr[format->a6.p1] = tmpRel;
                    cpu->regs.pr[format->a6.p2] = !tmpRel;
                }
            break;
        }
    } else {
        if (ctype == Unc) {
            if (format->a6.p1 == format->a6.p2) {
                cpu->IllegalOperationFault();
            }
            cpu->regs.pr[format->a6.p1] = 0;
            cpu->regs.pr[format->a6.p2] = 0;
        }
    }
}

// 4.2 page 272
//                  [op]
HandleFn handletable[8] = {
    ALUMMALU,   AddImm22,   UnimplInst, UnimplInst,
    Compare,    Compare,    Compare,    UnimplInst,
};

} // ALU
