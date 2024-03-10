#pragma once

#include "bundle.hpp"
#include "cpu.hpp"
#include "insttypes/alu.hpp"
#include "insttypes/addimm22.hpp"


namespace IUnit {

extern void Handle(Ia64Bundle *, Ia64Cpu *, uint64_t);

enum InstructionType {
    Misc_0          = 0x0,
    FaultyType_1    = 0x1,
    FaultyType_2    = 0x2,
    FaultyType_3    = 0x3,
    Deposit_4       = 0x4,
    ShiftTestBit_5  = 0x5,
    FaultyType_6    = 0x6,
    MMMpy_Shift_7   = 0x7, 
    ALUMM_ALU_8     = 0x8,
    AddImm22_9      = 0x9,
    FaultyType_A    = 0xa,
    FaultyType_B    = 0xb,
    Compare_C       = 0xc,
    Compare_D       = 0xd,
    Compare_E       = 0xe,
    FaultyType_F    = 0xf,
};

}
