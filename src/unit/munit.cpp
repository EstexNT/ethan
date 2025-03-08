#include "munit.hpp"

namespace MUnit {
static inline bool IsInterruptionCr(uint64_t cr) {
    return ((cr >= 16) && (cr <= 25));
}
static inline uint64_t ImplItirCwiMask(uint64_t a) {
    return a; // TODO
}

DECLINST(UnimplInst) {
    fprintf(stderr, "unimpl memory op=%d\n", format->common.op);
    cpu->halt = true;
}
DECLINST(UnimplInstOp) {
    fprintf(stderr, "unimpl memory op=%d x3=%d\n", format->common.op, format->m37.x3);
    cpu->halt = true;
}
DECLINST(UnimplInstOpX3) {
    fprintf(stderr, "unimpl memory op=%d x3=%d x4=%d x2=%d\n", format->common.op, format->m37.x3, format->m37.x4, format->m37.x2);
    cpu->halt = true;
}


// op = 0 //
DECLINST(SrlzD) {
    printf("(qp %d) srlz.d\n", format->m24.qp);
    if (ISQP(m24)) {
        // TODO:
        // cpu->DataSerialize();
    }
}
DECLINST(SrlzI) {
    printf("(qp %d) srlz.i\n", format->m24.qp);
    if (ISQP(m24)) {
        // TODO:
        // cpu->InstructionSerialize();
    }
}
DECLINST(NopM) {
    uint32_t imm = (format->m37.i << 20) | format->m37.imm20a;
    printf("(qp %d) nop.m 0x%08x\n", format->m37.qp, imm);
    if (ISQP(m37)) {
        // no operation
    }
}
DECLINST(Rsm) {
    uint32_t imm24 = (format->m44.i << 23) | (format->m44.i2d << 21) | (format->m44.imm21a);
    printf("(qp %d) rsm 0x%06x\n", format->m44.qp, imm24);
    if (ISQP(m44)) {
        if (cpu->regs.psr.cpl != 0) {
            cpu->PrivilegedOperationFault(0);
        }
        if (cpu->regs.IsReservedField(Ia64Regs::RegType::PSR_TYPE, 0, imm24)) {
            cpu->ReservedRegisterFieldFault();
        }

        // TODO: there is a better way to do this
        #define BLEH(bit) if (imm24 & bit) { \
            cpu->regs.psr.raw = cpu->regs.psr.raw & ~bit; \
        }

        BLEH(0b10);
        BLEH(0b100);
        BLEH(0b1000);
        BLEH(0b10000);
        BLEH(0b100000);
        BLEH(0b10000000000000);
        BLEH(0b100000000000000);
        BLEH(0b1000000000000000);
        BLEH(0b100000000000000000);
        BLEH(0b1000000000000000000);
        BLEH(0b10000000000000000000);
        BLEH(0b100000000000000000000);
        BLEH(0b1000000000000000000000);
        BLEH(0b10000000000000000000000);
        BLEH(0b100000000000000000000000);
#undef BLEH
    }
}
DECLINST(SyncI) {
    printf("(qp %d) sync.i\n", format->m24.qp);
    if (ISQP(m24)) {
        // TODO:
        // cpu->InstructionSynchronize();
    }
}

DECLINST(SysMemMgmt0Ext) {
    //                         [x4][x2]
    static HandleFn sysexttable[16][4] = {
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, SrlzD          },
        { NopM,           UnimplInstOpX3, UnimplInstOpX3, SrlzI          },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, SyncI          },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { Rsm,            Rsm,            Rsm,            Rsm            },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
    };
    sysexttable[format->m37.x4][format->m37.x2](format, cpu);
}

DECLINST(SysMemMgmt0) {
    static HandleFn op0handle[8] = {
        SysMemMgmt0Ext, UnimplInstOp, UnimplInstOp, UnimplInstOp,
        UnimplInstOp,   UnimplInstOp, UnimplInstOp, UnimplInstOp,
    };
    op0handle[format->m37.x3](format, cpu);
}


// op = 1 //

DECLINST(MovFromCPUID) {
    printf("(qp %d) mov r%d = cpuid[r%d]\n", format->m43.qp, format->m43.r1, format->m43.r3);
    if (ISQP(m43)) {
        uint8_t tmp_index = uint8_t(cpu->regs.gpr[format->m43.r3].val & 0xff);
        cpu->regs.CheckTargetRegister(format->m43.r1);
        if (cpu->regs.gpr[format->m43.r1].nat) {
            cpu->RegisterNatConsumptionFault(0);
        }
        if (cpu->regs.IsReservedReg(Ia64Regs::CPUID_TYPE, tmp_index)) {
            cpu->ReservedRegisterFieldFault();
        }
        cpu->regs.gpr[format->m43.r1] = cpu->regs.cpuid[tmp_index].val;
        cpu->regs.gpr[format->m43.r1] = false;
    }
}

DECLINST(MovMToAR) {
    printf("(qp %d) mov.m ar%d = r%d\n", format->m29.qp, format->m29.ar3, format->m29.r2);
    if (ISQP(m29)) {
        if (cpu->regs.IsReservedReg(Ia64Regs::AR_TYPE, format->m29.ar3)) {
            cpu->IllegalOperationFault();
        }
        uint64_t tmp_val = cpu->regs.gpr[format->m29.r2].val;
        if (cpu->regs.IsReadOnlyRegister(Ia64Regs::AR_TYPE, format->m29.ar3) || 
        (((format->m29.ar3 == Ia64Regs::Ar::Type::BSPSTORE) || (format->m29.ar3 == Ia64Regs::Ar::Type::RNAT)) && 
        (cpu->regs.ar[Ia64Regs::Ar::Type::RSC].val & 3))) {
            cpu->IllegalOperationFault();
        }
        if (cpu->regs.gpr[format->m29.r2].nat) {
            cpu->RegisterNatConsumptionFault(0);
        }
        if (cpu->regs.IsReservedField(Ia64Regs::AR_TYPE, format->m29.ar3, tmp_val)) {
            cpu->ReservedRegisterFieldFault();
        }
        if ((cpu->regs.IsKernelReg(format->m29.ar3) || format->m29.ar3 == Ia64Regs::Ar::Type::ITC) && 
        (cpu->regs.psr.cpl)) {
            cpu->PrivilegedRegisterFault();
        }
        if (!cpu->regs.IsIgnoredReg(format->m29.ar3)) {
            tmp_val = cpu->regs.IgnoredFieldMask(Ia64Regs::AR_TYPE, format->m29.ar3, tmp_val);
            if (format->m29.ar3 == Ia64Regs::Ar::Type::RSC && ((tmp_val >> 2) & 3) < cpu->regs.psr.cpl) {
                tmp_val |= (cpu->regs.psr.cpl << 2) & 3;
            }
            cpu->regs.ar[format->m29.ar3] = tmp_val;
            
            if (format->m29.ar3 == Ia64Regs::Ar::Type::BSPSTORE) {
                cpu->regs.ar[Ia64Regs::Ar::Type::BSP] = cpu->rse.UpdateInternalStackPointers(tmp_val);
                cpu->regs.ar[Ia64Regs::Ar::Type::RNAT] = 0; // undefined()
            }
        }
    }
}

DECLINST(MovFromMSR) {
    // not in the doc!
    printf("(qp %d) mov r%d = msr[r%d]\n", format->m43.qp, format->m43.r1, format->m43.r3);
    if (ISQP(m43)) {
        uint64_t tmp_index = cpu->regs.gpr[format->m43.r3].val;
        cpu->regs.CheckTargetRegister(format->m43.r1);
        if (cpu->regs.psr.cpl != 0) {
            cpu->PrivilegedOperationFault(0);
        }
        if (cpu->regs.gpr[format->m43.r1].nat) {
            cpu->RegisterNatConsumptionFault(0);
        }
        if (cpu->regs.IsReservedReg(Ia64Regs::MSR_TYPE, tmp_index)) {
            cpu->ReservedRegisterFieldFault();
        }
        cpu->regs.gpr[format->m43.r1] = cpu->regs.msr.Read(tmp_index);
        cpu->regs.gpr[format->m43.r1] = false;
    }
}
DECLINST(MovToMSR) {
    // not in the doc!
    printf("(qp %d) mov msr[r%d] = r%d\n", format->m42.qp, format->m42.r3, format->m42.r2);
    if (ISQP(m42)) {
        uint64_t tmp_index = cpu->regs.gpr[format->m42.r3].val;
        if (cpu->regs.psr.cpl != 0) {
            cpu->PrivilegedOperationFault(0);
        }
        if (cpu->regs.gpr[format->m42.r2].nat || cpu->regs.gpr[format->m42.r3].nat) {
            cpu->RegisterNatConsumptionFault(0);
        }
        if (cpu->regs.IsReservedReg(Ia64Regs::MSR_TYPE, tmp_index) || 
        cpu->regs.IsReservedField(Ia64Regs::MSR_TYPE, tmp_index, cpu->regs.gpr[format->m42.r2].val)) {
            cpu->ReservedRegisterFieldFault();
        }
        uint64_t tmpVal = cpu->regs.IgnoredFieldMask(Ia64Regs::MSR_TYPE, tmp_index, cpu->regs.gpr[format->m42.r2].val);
        cpu->regs.msr.Write(tmp_index, tmpVal);
    }
}
DECLINST(MovFromPSR) {
    printf("(qp %d) mov r%d = psr\n", format->m36.qp, format->m36.r1);
    if (ISQP(m36)) {
        cpu->regs.CheckTargetRegister(format->m36.r1);
        if (cpu->regs.psr.cpl != 0) {
            cpu->PrivilegedOperationFault(0);
        }
        uint64_t tmpVal = ZeroExt(cpu->regs.psr.raw & 0xffffffff, 32);
        tmpVal |= (((cpu->regs.psr.raw >> 35) & 0b11) << 35);
        cpu->regs.gpr[format->m36.r1] = tmpVal;
        cpu->regs.gpr[format->m36.r1] = false;
    }
}
DECLINST(MovFromCR) {
    printf("(qp %d) mov r%d = cr%d\n", format->m33.qp, format->m33.r1, format->m33.cr3);
    if (ISQP(m33)) {
        if (cpu->regs.IsReservedReg(Ia64Regs::RegType::CR_TYPE, format->m33.cr3) || 
        cpu->regs.psr.ic && IsInterruptionCr(format->m33.cr3)) {
            cpu->IllegalOperationFault();
        }
        cpu->regs.CheckTargetRegister(format->m33.r1);
        if (cpu->regs.psr.cpl != 0) {
            cpu->PrivilegedOperationFault(0);
        }
        if (format->m33.cr3 == Ia64Regs::Cr::Type::IVR) {
            cpu->CheckInterruptRequest();
        }
        if (format->m33.cr3 == Ia64Regs::Cr::Type::ITIR) {
            cpu->regs.gpr[format->m33.r1] = ImplItirCwiMask(cpu->regs.cr[format->m33.cr3].val);
        } else {
            cpu->regs.gpr[format->m33.r1] = cpu->regs.cr[format->m33.cr3].val;
        }
        cpu->regs.gpr[format->m33.r1] = false;
    }
}
DECLINST(MovToCR) {
    printf("(qp %d) mov cr%d = r%d\n", format->m32.qp, format->m32.cr3, format->m32.r2);
    if (ISQP(m32)) {
        if (cpu->regs.IsReservedReg(Ia64Regs::RegType::CR_TYPE, format->m32.cr3) || 
        cpu->regs.IsReadOnlyRegister(Ia64Regs::RegType::CR_TYPE, format->m32.cr3) ||
        cpu->regs.psr.ic && IsInterruptionCr(format->m32.cr3)) {
            cpu->IllegalOperationFault();
        }
        if (cpu->regs.psr.cpl != 0) {
            cpu->PrivilegedOperationFault(0);
        }
        if (cpu->regs.gpr[format->m32.r2].nat) {
            cpu->RegisterNatConsumptionFault(0);
        }
        if (cpu->regs.IsReservedField(Ia64Regs::RegType::CR_TYPE, format->m32.cr3, cpu->regs.gpr[format->m32.r2].val)) {
            cpu->ReservedRegisterFieldFault();
        }
        if (format->m32.cr3 == Ia64Regs::Cr::Type::EOI) {
            cpu->EndOfInterrupt();
        }
        uint64_t tmpVal = cpu->regs.IgnoredFieldMask(Ia64Regs::RegType::CR_TYPE, format->m32.cr3, cpu->regs.gpr[format->m32.r2].val);
        cpu->regs.cr[format->m32.cr3] = tmpVal;
        if (format->m32.cr3 == Ia64Regs::Cr::Type::IIPA) {
            //last_IP = tmpVal;
        }
    }
}
DECLINST(MovMFromAR) {
    printf("(qp %d) mov.m r%d = ar%d\n", format->m31.qp, format->m31.r1, format->m31.ar3);
    if (ISQP(m31)) {
        if (cpu->regs.IsReservedReg(Ia64Regs::AR_TYPE, format->m31.ar3)) {
            cpu->IllegalOperationFault();
        }
        cpu->regs.CheckTargetRegister(format->m31.r1);
        if (((format->m31.ar3 == Ia64Regs::Ar::BSPSTORE) || (format->m31.ar3 == Ia64Regs::Ar::RNAT)) && 
        (cpu->regs.ar[Ia64Regs::Ar::Type::RSC].val & 3)) {
            cpu->IllegalOperationFault();
        }
        if (format->m31.ar3 == Ia64Regs::Ar::ITC && cpu->regs.psr.si && cpu->regs.psr.cpl != 0) {
            cpu->PrivilegedRegisterFault();
        }
        cpu->regs.gpr[format->m31.r1] = (cpu->regs.IsIgnoredReg(format->m31.ar3)) ? 0 : cpu->regs.ar[format->m31.ar3].val;
        cpu->regs.gpr[format->m31.r1] = false;
    }
}


DECLINST(SysMemMgmt1Ext) {
    static HandleFn sysexttable1[16][4] = {
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, MovMFromAR,     UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, MovFromCR,      UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, MovFromPSR,     UnimplInstOpX3 },
        { MovToMSR,       MovFromMSR,     UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, MovFromCPUID,   UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, MovMToAR,       UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, MovToCR,        UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
    };
    sysexttable1[format->m29.x6 & 0xf][(format->m29.x6 >> 4) & 3](format, cpu);
}

DECLINST(SysMemMgmt1) {
    static HandleFn op1handle[8] = {
        SysMemMgmt1Ext, UnimplInstOp, UnimplInstOp, UnimplInstOp,
        UnimplInstOp,   UnimplInstOp, UnimplInstOp, UnimplInstOp,
    };
    op1handle[format->m37.x3](format, cpu);
}


// 4.4 page 296
//                  [op]
HandleFn handletable[8] = {
    SysMemMgmt0, SysMemMgmt1, UnimplInst, UnimplInst, 
    UnimplInst,  UnimplInst,  UnimplInst, UnimplInst, 
};


} // MUnit
