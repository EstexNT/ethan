#pragma once
#include "cpu.hpp"
#include "hint.hpp"
#include "insttypes/alu.hpp"
#include "insttypes/addimm22.hpp"
#include "insttypes/compare.hpp"


namespace MUnit {
extern void Handle(Ia64Bundle *, Ia64Cpu *, uint64_t);

enum InstructionType {
    SysMem_Mgmt_0   = 0x0,
    SysMem_Mgmt_1   = 0x1,
    FaultyType_2    = 0x2,
    FaultyType_3    = 0x3,
    IntLd_RegGetf_4 = 0x4,
    IntLd_StImm_5   = 0x5,
    FPLd_StReg_Setf_6=0x6,
    FPLd_StImm_7    = 0x7, 
    ALU_MMALU_8     = 0x8,
    AddImm22_9      = 0x9,
    FaultyType_A    = 0xa,
    FaultyType_B    = 0xb,
    Compare_C       = 0xc,
    Compare_D       = 0xd,
    Compare_E       = 0xe,
    FaultyType_F    = 0xf,
};

}
