#pragma once

#include "cpu.hpp"

namespace FUnit { // not FUn at all


extern HandleFn handletable[0x10];

static inline HandleFn Handle(Ia64Format *format, Ia64Cpu *cpu) {
    return handletable[format->common.op];
}

}
