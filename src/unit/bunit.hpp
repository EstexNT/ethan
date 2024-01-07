#pragma once

#include "common.h"
#include "bundle.hpp"
#include "cpu.hpp"

namespace BUnit {

static inline uint64_t AlignIP(uint64_t in) {
    return in & ~0xfUL;
}

extern void Handle(Ia64Bundle *, Ia64Cpu *, uint64_t);

enum InstructionType {
    Misc_IndirectBranch_0 = 0x0,
    IndirectCall_1        = 0x1,
    IndirectPredict_Nop_2 = 0x2,
    Ignored_3             = 0x3,
    IPRelativeBranch_4    = 0x4,
    IPRelativeCall_5      = 0x5,
    Ignored_6             = 0x6,
    IPRelativePredict_7   = 0x7,
    SpecialE_8            = 0x8,
    SpecialE_9            = 0x9,
    SpecialE_A            = 0xa,
    SpecialE_B            = 0xb,
    SpecialE_C            = 0xc,
    SpecialE_D            = 0xd,
    SpecialE_E            = 0xe,
    SpecialE_F            = 0xf,
};

} // BUnit