#include "munit.hpp"

namespace MUnit {

static inline uint64_t IgnoredFieldMask(Ia64Regs::RegType type, uint64_t idx, uint64_t val) {
    switch (type) {
        case Ia64Regs::AR_TYPE:
            if ((idx >= 0) && (idx <= 7)) {
                // kernel registers. no ignored fields
                return val;
            }
            break;
        case Ia64Regs::MSR_TYPE:
            // probably has no ignored fields
            return val;
    }
    debugprintf("TODO: ignored_field_mask(%d, %ld, %ld)\n", (int)type, idx, val);
    return val;
}
static inline bool IsReservedReg(Ia64Regs::RegType type, uint64_t ar, Ia64Cpu *cpu) {
    switch (type) {
        case Ia64Regs::CPUID_TYPE:
            if (ar >= (cpu->regs.cpuid[3].val & 0xff)) {
                return true;
            }
            return false;

        case Ia64Regs::AR_TYPE:
            if ((ar >= 8) && (ar <= 15)) {
                return true;
            }
            if (ar == 20) {
                return true;
            }
            if ((ar == 22) || (ar == 23)) {
                return true;
            }
            if (ar == 31) {
                return true;
            }
            if ((ar == 33) || (ar == 34) || (ar == 35)) {
                return true;
            }
            if ((ar == 37) || (ar == 38) || (ar == 39)) {
                return true;
            }
            if ((ar == 41) || (ar == 42) || (ar == 43)) {
                return true;
            }
            if ((ar == 45) || (ar == 46) || (ar == 47)) {
                return true;
            }
            if ((ar >= 67) && (ar <= 111)) {
                return true;
            }
            break;

        case Ia64Regs::MSR_TYPE:
            return false;
        
        default:
            debugprintf("TODO: is_reserved_reg(%d, %lx)\n", (int)type, ar);
            return false;
    }
    return false;
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
DECLINST(SrlzI) {
    printf("(qp %d) srlz.i\n", format->m24.qp);
    if (cpu->regs.pr[format->m24.qp].val) {
        // TODO:
        // cpu->InstructionSerialize();
    }
}
DECLINST(NopM) {
    uint32_t imm = (format->m37.i << 20) | format->m37.imm20a;
    printf("(qp %d) nop.m 0x%08x\n", format->m37.qp, imm);
    if (cpu->regs.pr[format->m37.qp].val) {
        // no operation
    }
}

DECLINST(SysMemMgmt0Ext) {
    //                         [x4][x2]
    static HandleFn sysexttable[16][4] = {
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { NopM,           UnimplInstOpX3, UnimplInstOpX3, SrlzI          },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
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
    if (cpu->regs.pr[format->m43.qp].val) {
        uint8_t tmp_index = uint8_t(cpu->regs.gpr[format->m43.r3].val & 0xff);
        cpu->regs.CheckTargetRegister(format->m43.r1);
        if (cpu->regs.gpr[format->m43.r1].nat) {
            cpu->RegisterNatConsumptionFault(0);
        }
        if (IsReservedReg(Ia64Regs::CPUID_TYPE, tmp_index, cpu)) {
            cpu->ReservedRegisterFieldFault();
        }
        cpu->regs.gpr[format->m43.r1] = cpu->regs.cpuid[tmp_index].val;
        cpu->regs.gpr[format->m43.r1] = false;
    }
}

DECLINST(MovMToAR) {
    printf("(qp %d) mov.m ar%d = r%d\n", format->m29.qp, format->m29.ar3, format->m29.r2);
    if (cpu->regs.pr[format->m29.qp].val) {
        if (IsReservedReg(Ia64Regs::AR_TYPE, format->m29.ar3, cpu)) {
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
            tmp_val = IgnoredFieldMask(Ia64Regs::AR_TYPE, format->m29.ar3, tmp_val);
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
    if (cpu->regs.pr[format->m43.qp].val) {
        uint64_t tmp_index = cpu->regs.gpr[format->m43.r3].val;
        cpu->regs.CheckTargetRegister(format->m43.r1);
        if (cpu->regs.psr.cpl != 0) {
            cpu->PrivilegedOperationFault(0);
        }
        if (cpu->regs.gpr[format->m43.r1].nat) {
            cpu->RegisterNatConsumptionFault(0);
        }
        if (IsReservedReg(Ia64Regs::MSR_TYPE, tmp_index, cpu)) {
            cpu->ReservedRegisterFieldFault();
        }
        cpu->regs.gpr[format->m43.r1] = cpu->regs.msr.Read(tmp_index);
        cpu->regs.gpr[format->m43.r1] = false;
    }
}
DECLINST(MovToMSR) {
    // not in the doc!
    printf("(qp %d) mov msr[r%d] = r%d\n", format->m42.qp, format->m42.r3, format->m42.r2);
    if (cpu->regs.pr[format->m42.qp].val) {
        uint64_t tmp_index = cpu->regs.gpr[format->m42.r3].val;
        if (cpu->regs.psr.cpl != 0) {
            cpu->PrivilegedOperationFault(0);
        }
        if (cpu->regs.gpr[format->m42.r2].nat || cpu->regs.gpr[format->m42.r3].nat) {
            cpu->RegisterNatConsumptionFault(0);
        }
        if (IsReservedReg(Ia64Regs::MSR_TYPE, tmp_index, cpu) || 
        cpu->regs.IsReservedField(Ia64Regs::MSR_TYPE, tmp_index, cpu->regs.gpr[format->m42.r2].val)) {
            cpu->ReservedRegisterFieldFault();
        }
        uint64_t tmpVal = IgnoredFieldMask(Ia64Regs::MSR_TYPE, tmp_index, cpu->regs.gpr[format->m42.r2].val);
        cpu->regs.msr.Write(tmp_index, tmpVal);
    }
}


DECLINST(SysMemMgmt1Ext) {
    static HandleFn sysexttable1[16][4] = {
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { MovToMSR,       MovFromMSR,     UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, MovFromCPUID,   UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, MovMToAR,       UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
        { UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3, UnimplInstOpX3 },
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
