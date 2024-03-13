#pragma once

#include "cpu.hpp"

namespace BUnit {

//                         [op]
extern HandleFn handletable[8];

static inline uint64_t AlignIP(uint64_t in) {
    return in & uint64_t(~(IA64BUNDLESIZE - 1));
}

static inline HandleFn Handle(Ia64Format *format, Ia64Cpu *cpu) {
    return handletable[format->common.op];
}


} // BUnit
