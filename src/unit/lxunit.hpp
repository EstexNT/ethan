#pragma once

#include <utility>
#include "cpu.hpp"

namespace LXUnit {

//                         [op]
extern HandleFn handletable[16];

static inline std::pair<HandleFn, HandleFn> Handle(Ia64Format *format0, Ia64Format *format1, Ia64Cpu *cpu) {
    std::pair<HandleFn, HandleFn> out { handletable[format0->common.op], handletable[format0->common.op] };
    return out;
}


} // LXUnit
