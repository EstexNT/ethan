#pragma once

#include "cpu.hpp"


namespace IUnit {

//                         [op]
extern HandleFn handletable[8];

static inline HandleFn Handle(Ia64Format *format, Ia64Cpu *cpu) {
    return handletable[format->common.op];
}


} // IUnit
