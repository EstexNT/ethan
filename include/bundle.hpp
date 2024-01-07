#pragma once
#include <stdint.h>
#include "n128.h"

class Ia64Cpu;
struct Ia64Bundle {
    #pragma pack(push,1)
    union {
        struct {
            uint128_t _template:5;
            uint128_t _instslot0:41;
            uint128_t _instslot1:41;
            uint128_t _instslot2:41;
        };
        uint128_t raw;
    };
    #pragma pack(pop)
    
    Ia64Bundle(uint128_t d) {
        raw = d;
    }
    Ia64Bundle(uint8_t tmplt, uint64_t is0, uint64_t is1, uint64_t is2) {
        _template = tmplt;
        _instslot0 = is0;
        _instslot1 = is1;
        _instslot2 = is2;
    }

    void Handle(Ia64Cpu *cpu);
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