#pragma once

#include "cpu.hpp"
#include "insttype/alu.hpp"

namespace MUnit {

//                          [op]
extern HandleFn handletable[8];

static inline HandleFn Handle(Ia64Format *format, Ia64Cpu *cpu) {
    if (format->common.op >= 8) {
        return ALU::Handle(format, cpu);
    }
    return handletable[format->common.op];
}


} // MUnit
