#pragma once

#include "common.hpp"
#include "bundle.hpp"
#include "cpu.hpp"

namespace LXUnit {

extern void Handle(Ia64Bundle *, Ia64Cpu *, uint64_t, uint64_t);

enum InstructionType {
    Misc_0 = 0,
    FaultyType_1 = 1,
    FaultyType_2 = 2,
    FaultyType_3 = 3,
    FaultyType_4 = 4,
    FaultyType_5 = 5,
    Movl_6 = 6,
    FaultyType_7 = 7,
    FaultyType_8 = 8,
    FaultyType_9 = 9,
    FaultyType_A = 0xa,
    FaultyType_B = 0xb,
    LongBranch_C = 0xc,
    LongCall_D = 0xd,
    FaultyType_E = 0xe,
    FaultyType_F = 0xf,
};

} // LXUnit
