#pragma once

#include "cpu.hpp"

namespace LXUnit {

//                         [op]
extern LXHandleFn handletable[16];

static inline HandleFn Handle(Ia64Format *format0, Ia64Format *format1, Ia64Cpu *cpu) {
    return (HandleFn)handletable[format1->common.op];
}

#define DECLINSTLX(name) static void name(Ia64Format *format0, Ia64Format *format1, Ia64Cpu *cpu)


} // LXUnit
