#include "compare.hpp"

namespace Compare {
#define DECLINST(name) static void name(Ia64Bundle *bundle, Ia64Cpu *cpu, uint64_t slot)
#define DEFINEA6VARS \
    uint8_t p2 = (slot >> 27) & 63; \
    uint8_t r3 = (slot >> 20) & 127; \
    uint8_t r2 = (slot >> 13) & 127; \
    uint8_t p1 = (slot >> 6) & 63; \
    uint8_t qp = (slot >> 0) & 63; 
    
    

DECLINST(UnimplInst) {
    uint8_t op = (slot >> 37) & 0xf;
    uint8_t x2 = (slot >> 34) & 3;
    uint8_t tb = (slot >> 36) & 1;
    uint8_t ta = (slot >> 33) & 1;
    uint8_t c = (slot >> 12) & 1;
    fprintf(stderr, "unimpl compare op=%x x2=%x tb=%x ta=%x c=%x\n", op, x2, tb, ta, c);
    cpu->halt = true;
}

DECLINST(CmpEqUnc) {
    // page 46
    DEFINEA6VARS;
    //printf("(qp %d) adds r%d = %ld, r%d\n", qp, r1, imma, r3);
    printf("(qp %d) cmp.eq.unc p%d, p%d = r%d, r%d\n", qp, p1, p2, r2, r3);
    if (cpu->branched) {
        return;
    }
    if (cpu->regs.pr[qp].val) {
        if (p1 == p2) {
            cpu->IllegalOperationFault();
        }
        bool tmp_nat = (cpu->regs.gpr[r2].nat) || (cpu->regs.gpr[r3].nat);
        uint64_t tmp_src = cpu->regs.gpr[r2].val;
        bool tmp_rel = tmp_src == cpu->regs.gpr[r3].val;
        if (tmp_nat) {
            cpu->regs.pr[p1] = 0;
            cpu->regs.pr[p2] = 0;
        } else {
            cpu->regs.pr[p1] = tmp_rel;
            cpu->regs.pr[p2] = !tmp_rel;
        }
    } else {
        if (p1 == p2) {
            cpu->IllegalOperationFault();
            cpu->regs.pr[p1] = 0;
            cpu->regs.pr[p2] = 0;
            
        }
    }
}

// 4.2.2 page 275 
// [op][x2][tb][ta][c]
void (*handletable[3][4][2][2][2])(Ia64Bundle *, Ia64Cpu *, uint64_t) = {
        { // [0][0]xxx
        { { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
          { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
        }, // [0][1]xxx
        { { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
          { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
        }, // [0][2]xxx
        { { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
          { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
        }, // [0][3]xxx
        { { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
          { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
        },
        },
        { // [1][0]xxx
        { { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
          { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
        }, // [1][1]xxx
        { { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
          { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
        }, // [1][2]xxx
        { { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
          { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
        }, // [1][3]xxx
        { { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
          { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
        },
        },
        { // [2][0]xxx
        { { {UnimplInst, CmpEqUnc}, {UnimplInst, UnimplInst}, }, 
          { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
        }, // [2][1]xxx
        { { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
          { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
        }, // [2][2]xxx
        { { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
          { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
        }, // [2][3]xxx
        { { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
          { {UnimplInst, UnimplInst}, {UnimplInst, UnimplInst}, }, 
        },
        },
};

} // Compare