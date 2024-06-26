#pragma once

#include <array>
#include "common.hpp"

struct Ia64Format;
class Ia64Cpu;
typedef void (*HandleFn)(Ia64Format *, Ia64Cpu *);
typedef void (*LXHandleFn)(Ia64Format *, Ia64Format *, Ia64Cpu *);

struct Ia64Bundle {
    #define IA64BUNDLESIZE 0x10
    #pragma pack(push,1)
    union {
        struct {
            uint64_t _template:5;
            uint64_t _instslot0:41;
            uint64_t _instslot1:41;
            uint64_t _instslot2:41;
        };
        uint64_t raw[2];
    };
    #pragma pack(pop)
    
    Ia64Bundle(void) {
        raw[0] = raw[1] = 0;
    }

    std::array<HandleFn, 3> Handle(Ia64Cpu *cpu);
};

enum Ia64BundleTemplate {
    MII_0  = 0x0,
    MII_1  = 0x1,
    MII_2  = 0x2,
    MII_3  = 0x3,
    MLX_4  = 0x4,
    MLX_5  = 0x5,
    BAD_6  = 0x6,
    BAD_7  = 0x7,
    MMI_8  = 0x8,
    MMI_9  = 0x9,
    MMI_A  = 0xa,
    MMI_B  = 0xb,
    MFI_C  = 0xc,
    MFI_D  = 0xd,
    MMF_E  = 0xe,
    MMF_F  = 0xf,
    MIB_10 = 0x10,
    MIB_11 = 0x11,
    MBB_12 = 0x12,
    MBB_13 = 0x13,
    BAD_14 = 0x14,
    BAD_15 = 0x15,
    BBB_16 = 0x16,
    BBB_17 = 0x17,
    MMB_18 = 0x18,
    MMB_19 = 0x19,
    BAD_1A = 0x1a,
    BAD_1B = 0x1b,
    MFB_1C = 0x1c,
    MFB_1D = 0x1d,
    BAD_1E = 0x1e,
    BAD_1F = 0x1f,
};
