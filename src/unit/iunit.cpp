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
DECLINST(UnimplInstOp5) {
    fprintf(stderr, "unimpl iunit op=5 x=%d y=%d x2=%d\n", format->i11.x, format->i11.y, format->i11.x2);
    cpu->halt = true;
}
DECLINST(UnimplInstTbit) {
    fprintf(stderr, "unimpl iunit testbit ta=%d tb=%d c=%d y=%d\n", format->i16.ta, format->i16.tb, format->i16.c, format->i16.y);
    cpu->halt = true;
}

// op = 0 //


DECLINST(MovFromIP) {
    printf("(qp %d) mov r%d = ip\n", format->i25.qp, format->i25.r1);
    if (ISQP(i25)) {
        cpu->regs.CheckTargetRegister(format->i25.r1);
        cpu->regs.gpr[format->i25.r1] = cpu->regs.ip;
        cpu->regs.gpr[format->i25.r1] = false;
    }
}
DECLINST(NopI) {
    uint32_t imm = (format->i19.i << 20) | format->i19.imm20a;
    printf("(qp %d) nop.i 0x%08x\n", format->i19.qp, imm);
    if (ISQP(i19)) {
        // no operation
    }
}
DECLINST(MovFromB) {
    printf("(qp %d) mov r%d = b%d\n", format->i22.qp, format->i22.r1, format->i22.b2);
    if (ISQP(i22)) {
        cpu->regs.CheckTargetRegister(format->i22.r1);
        cpu->regs.gpr[format->i22.r1] = cpu->regs.br[format->i22.b2].val;
        cpu->regs.gpr[format->i22.r1] = false;
    }
}
DECLINST(MoviFromAr) {
    printf("(qp %d) mov.i r%d = ar%d\n", format->i28.qp, format->i28.r1, format->i28.ar3);
    if (ISQP(i28)) {
        if (cpu->regs.IsReservedReg(Ia64Regs::AR_I_TYPE, format->i28.ar3)) {
            cpu->IllegalOperationFault();
        }
        cpu->regs.CheckTargetRegister(format->i28.r1);
        if (((format->i28.ar3 == Ia64Regs::Ar::BSPSTORE) || (format->i28.ar3 == Ia64Regs::Ar::RNAT)) && ((cpu->regs.ar[Ia64Regs::Ar::RSC].val & 0b11) != 0)) {
            cpu->IllegalOperationFault();
        }
        if (format->i28.ar3 == Ia64Regs::Ar::ITC && cpu->regs.psr.si && cpu->regs.psr.cpl != 0) {
            cpu->PrivilegedRegisterFault();
        }
        cpu->regs.gpr[format->i28.r1] = (cpu->regs.IsIgnoredReg(format->i28.ar3)) ? 0 : cpu->regs.ar[format->i28.ar3].val;
        cpu->regs.gpr[format->i28.r1] = false;
    }
}
DECLINST(MoviToArImm8) {
    uint8_t imm8 = (format->i27.s << 7)| (format->i27.imm7b);
    printf("(qp %d) mov.i ar%d = 0x%02x\n", format->i27.qp, format->i27.ar3, imm8);
    if (ISQP(i27)) {
        if (cpu->regs.IsReservedReg(Ia64Regs::AR_I_TYPE, format->i27.ar3)) {
            cpu->IllegalOperationFault();
        }
        uint64_t tmpVal = SignExt(imm8, 8);
        if (cpu->regs.IsReadOnlyRegister(Ia64Regs::AR_TYPE, format->i27.ar3) || 
            ((format->i27.ar3 == Ia64Regs::Ar::BSPSTORE) || (format->i27.ar3 == Ia64Regs::Ar::RNAT)) && ((cpu->regs.ar[Ia64Regs::Ar::RSC].val & 0b11) != 0)) {
            cpu->IllegalOperationFault();
        }
        if (cpu->regs.IsReservedField(Ia64Regs::AR_TYPE, format->i27.ar3, tmpVal)) {
            cpu->ReservedRegisterFieldFault();
        }
        if ((cpu->regs.IsKernelReg(format->i27.ar3) || (format->i27.ar3 == Ia64Regs::Ar::ITC)) && (cpu->regs.psr.cpl != 0)) {
            cpu->PrivilegedRegisterFault();
        }
        if (!cpu->regs.IsIgnoredReg(format->i27.ar3)) {
            tmpVal = cpu->regs.IgnoredFieldMask(Ia64Regs::AR_TYPE, format->i27.ar3, tmpVal);
            if (format->i27.ar3 == Ia64Regs::Ar::RSC && (((tmpVal >> 2) & 0b11) < cpu->regs.psr.cpl)) {
                tmpVal = (tmpVal & ~(0b11 << 2)) | (cpu->regs.psr.cpl << 2);
            }
            cpu->regs.ar[format->i27.ar3] = tmpVal;
            if (format->i27.ar3 == Ia64Regs::Ar::BSPSTORE) {
                cpu->regs.ar[Ia64Regs::Ar::BSP] = cpu->rse.UpdateInternalStackPointers(tmpVal);
                cpu->regs.ar[Ia64Regs::Ar::RNAT] = 0; // undefined()
            }
        }
    }
}
DECLINST(MoviToAr) {
    printf("(qp %d) mov.i ar%d = r%d\n", format->i26.qp, format->i26.ar3, format->i26.r2);
    if (ISQP(i26)) {
        if (cpu->regs.IsReservedReg(Ia64Regs::AR_I_TYPE, format->i26.ar3)) {
            cpu->IllegalOperationFault();
        }
        uint64_t tmpVal = cpu->regs.gpr[format->i26.r2].val;
        if (cpu->regs.IsReadOnlyRegister(Ia64Regs::AR_TYPE, format->i26.ar3) || 
            ((format->i26.ar3 == Ia64Regs::Ar::BSPSTORE) || (format->i26.ar3 == Ia64Regs::Ar::RNAT)) && ((cpu->regs.ar[Ia64Regs::Ar::RSC].val & 0b11) != 0)) {
            cpu->IllegalOperationFault();
        }
        if (cpu->regs.gpr[format->i26.r2].nat) {
            cpu->RegisterNatConsumptionFault(0);
        }
        if (cpu->regs.IsReservedField(Ia64Regs::AR_TYPE, format->i26.ar3, tmpVal)) {
            cpu->ReservedRegisterFieldFault();
        }
        if ((cpu->regs.IsKernelReg(format->i26.ar3) || (format->i26.ar3 == Ia64Regs::Ar::ITC)) && (cpu->regs.psr.cpl != 0)) {
            cpu->PrivilegedRegisterFault();
        }
        if (!cpu->regs.IsIgnoredReg(format->i26.ar3)) {
            tmpVal = cpu->regs.IgnoredFieldMask(Ia64Regs::AR_TYPE, format->i26.ar3, tmpVal);
            if (format->i26.ar3 == Ia64Regs::Ar::RSC && (((tmpVal >> 2) & 0b11) < cpu->regs.psr.cpl)) {
                tmpVal = (tmpVal & ~(0b11 << 2)) | (cpu->regs.psr.cpl << 2);
            }
            cpu->regs.ar[format->i26.ar3] = tmpVal;
            if (format->i26.ar3 == Ia64Regs::Ar::BSPSTORE) {
                cpu->regs.ar[Ia64Regs::Ar::BSP] = cpu->rse.UpdateInternalStackPointers(tmpVal);
                cpu->regs.ar[Ia64Regs::Ar::RNAT] = 0; // undefined()
            }
        }
    }
}

DECLINST(MiscExt) {
    static HandleFn miscexthandle[16][4] = {
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, MovFromIP        },
        { NopI,             UnimplInstOp0X30, UnimplInstOp0X30, MovFromB         },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, MoviFromAr       },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30, UnimplInstOp0X30 },
        { MoviToArImm8,     UnimplInstOp0X30, MoviToAr,         UnimplInstOp0X30 },
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
    if (ISQP(i21)) {
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
        MiscExt,       UnimplInstOp0, UnimplInstOp0, UnimplInstOp0,
        UnimplInstOp0, UnimplInstOp0, UnimplInstOp0, MovToB,
    };
    op0handle[format->i19.x3](format, cpu);
}


// op = 4 //
DECLINST(Deposit) {
    uint64_t len4 = format->i15.len4d + 1;
    uint64_t pos6 = 63 - format->i15.cpos6d;

    //merge_form, register_form

    printf("(qp %d) dep r%d = r%d, r%d, %ld, %ld\n", format->i15.qp, format->i15.r1, format->i15.r2, format->i15.r3, pos6, len4);
    if (ISQP(i15)) {
        cpu->regs.CheckTargetRegister(format->i15.r1);

        uint64_t tmpLen = len4;
        if ((pos6 + tmpLen) > 64) {
            tmpLen = 64 - pos6;
        }
        uint64_t mask = (1 << tmpLen) - 1;
        cpu->regs.gpr[format->i15.r1] = ((cpu->regs.gpr[format->i15.r2].val & mask) << pos6) 
                                      | ((cpu->regs.gpr[format->i15.r3].val & ~mask) << pos6);
        cpu->regs.gpr[format->i15.r1] = cpu->regs.gpr[format->i15.r3].nat || cpu->regs.gpr[format->i15.r2].nat;
    }
}


// op = 5 //

DECLINST(ExtrU) {
    printf("(qp %d) extr.u r%d = r%d, %d, %d\n", format->i11.qp, format->i11.r1, format->i11.r3, format->i11.pos6b, format->i11.len6d + 1);
    if (ISQP(i11)) {
        cpu->regs.CheckTargetRegister(format->i11.r1);
        
        uint32_t tmp_len = format->i11.len6d;
        if ((format->i11.pos6b + tmp_len) > 64) {
            tmp_len = 64 - format->i11.pos6b;
        }
        cpu->regs.gpr[format->i11.r1] = ZeroExt(cpu->regs.gpr[format->i11.r3].val >> format->i11.pos6b, tmp_len);
        cpu->regs.gpr[format->i11.r1] = cpu->regs.gpr[format->i11.r1].nat;
    }
}

DECLINST(DepImm1) {
    uint64_t len4 = format->i14.len6d + 1;
    uint64_t pos6 = 63 - format->i14.cpos6b;
    uint64_t imm1 = SignExt(format->i14.s, 1);

    printf("(qp %d) dep r%d = %ld, r%d, %ld, %ld\n", format->i14.qp, format->i14.r1, imm1, format->i14.r3, pos6, len4);
    if (ISQP(i14)) {
        cpu->regs.CheckTargetRegister(format->i14.r1);

        uint64_t tmpLen = len4;
        if ((pos6 + tmpLen) > 64) {
            tmpLen = 64 - pos6;
        }
        uint64_t mask = (1 << tmpLen) - 1;
        cpu->regs.gpr[format->i14.r1] = (cpu->regs.gpr[format->i14.r3].val & ~(mask << pos6))
                                        | ((imm1 & mask) << tmpLen);
        cpu->regs.gpr[format->i14.r1] = cpu->regs.gpr[format->i14.r3].nat;
    
    }
}

DECLINST(DepZImm8) {
    uint64_t len6 = format->i13.len6d + 1;
    uint64_t pos6 = 63 - format->i13.cpos6c;
    uint64_t imm8 = SignExt((format->i13.s << 7) | (format->i13.imm7b), 8);

    printf("(qp %d) dep.z r%d = %ld, %ld, %ld\n", format->i13.qp, format->i13.r1, imm8, pos6, len6);
    if (ISQP(i13)) {
        cpu->regs.CheckTargetRegister(format->i13.r1);

        uint64_t tmpLen = len6;
        if ((pos6 + tmpLen) > 64) {
            tmpLen = 64 - pos6;
        }
        uint64_t mask = (1 << tmpLen) - 1;
        cpu->regs.gpr[format->i13.r1] = ((imm8 & mask) << tmpLen);
        cpu->regs.gpr[format->i13.r1] = false;
    }
}


DECLINST(TBitZ) {
    printf("(qp %d) tbit.z p%d, p%d = r%d, %d\n", format->i16.qp, format->i16.p1, format->i16.p2, format->i16.r3, format->i16.pos6b);

    if (ISQP(i16)) {
        if (format->i16.p1 == format->i16.p2) {
            cpu->IllegalOperationFault();
        }

        bool tmp_rel = (cpu->regs.gpr[format->i16.r3].val & (1 << format->i16.pos6b)) == 0;
        if (cpu->regs.gpr[format->i16.r3].nat) {
            cpu->regs.pr[format->i16.p1] = 0;
            cpu->regs.pr[format->i16.p2] = 0;
        } else {
            cpu->regs.pr[format->i16.p1] = tmp_rel;
            cpu->regs.pr[format->i16.p2] = !tmp_rel;
        }
    }
}

DECLINST(TestBit) {
    //                           [ta][tb][c][y]
    static HandleFn testbithandle[2][2][2][2] = {
        {
        { {TBitZ,          UnimplInstTbit}, {UnimplInstTbit, UnimplInstTbit} },
        { {UnimplInstTbit, UnimplInstTbit}, {UnimplInstTbit, UnimplInstTbit} },
        },
        {
        { {UnimplInstTbit, UnimplInstTbit}, {UnimplInstTbit, UnimplInstTbit} },
        { {UnimplInstTbit, UnimplInstTbit}, {UnimplInstTbit, UnimplInstTbit} },
        },
    };

    testbithandle[format->i16.ta][format->i16.tb][format->i16.c][format->i16.y](format, cpu);
}

DECLINST(ShiftTestBit) {
    static HandleFn op5handle[2][2][4] = {
        {
            {TestBit,       ExtrU,         UnimplInstOp5, UnimplInstOp5},
            {UnimplInstOp5, UnimplInstOp5, UnimplInstOp5, UnimplInstOp5},
        },
        {
            {UnimplInstOp5, UnimplInstOp5, UnimplInstOp5, DepImm1      },
            {UnimplInstOp5, DepZImm8,      UnimplInstOp5, UnimplInstOp5},
        },
        
    };
    op5handle[format->i11.x][format->i11.y][format->i11.x2](format, cpu);
}


// 4.3 page 282
//                  [op]
HandleFn handletable[8] = {
    Misc,       UnimplInst,   UnimplInst, UnimplInst,
    Deposit,    ShiftTestBit, UnimplInst, UnimplInst,
};


} // IUnit
