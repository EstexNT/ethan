#pragma once
#include "cpu.hpp"
#include "hint.hpp"


namespace MUnit {

//                          [op]
extern HandleFn handletable[8];

static inline HandleFn Handle(Ia64Format *format, Ia64Cpu *cpu) {
    return handletable[format->common.op];
}


} // MUnit
