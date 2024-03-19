#pragma once

#include "common.hpp"
#include "cpu.hpp"

namespace ALU {

//                         [op]
extern HandleFn handletable[8];

static inline HandleFn Handle(Ia64Format *format, Ia64Cpu *cpu) {
    return handletable[format->common.op - 8];
}

} // ALU
