#pragma once
#include "common.hpp"
#include "bundle.hpp"
#include "cpu.hpp"

namespace Compare {
//                                                          [op][x2][tb][ta][c]
extern void (*handletable[3][4][2][2][2])(Ia64Bundle *, Ia64Cpu *, uint64_t);

static inline void Handle(Ia64Bundle *bundle, Ia64Cpu *cpu, uint64_t slot) {
    uint8_t op = ((slot >> 37) & 0xf) - 0xc;
    uint8_t x2 = (slot >> 34) & 3;
    uint8_t tb = (slot >> 36) & 1;
    uint8_t ta = (slot >> 33) & 1;
    uint8_t c = (slot >> 12) & 1;

    handletable[op][x2][tb][ta][c](bundle, cpu, slot);
}

} // Compare
