#pragma once
#include "common.hpp"
#include "bundle.hpp"

// hmm.. could be better?
#pragma pack(push,1)

struct Ia64Format {
    union {


        // Common //
        struct {
            uint64_t:37;
            uint64_t op:4;
        } common;


        // A //
        struct {
            uint64_t qp:6;
            uint64_t r1:7;
            uint64_t r2:7;
            uint64_t r3:7;
            uint64_t x2b:2;
            uint64_t x4:4;
            uint64_t ve:1;
            uint64_t x2a:2;
            uint64_t:1;
            uint64_t const8:4;
        } a1; // ALU
        struct {
            uint64_t qp:6;
            uint64_t r1:7;
            uint64_t imm7b:7;
            uint64_t r3:7;
            uint64_t imm6d:6;
            uint64_t ve:1;
            uint64_t x2a:2;
            uint64_t s:1;
            uint64_t const8:4;
        } a4; // Add Imm14
        struct {
            uint64_t qp:6;
            uint64_t r1:7;
            uint64_t imm7b:7;
            uint64_t r3:2;
            uint64_t imm5c:5;
            uint64_t imm9d:9;
            uint64_t s:1;
            uint64_t const9:4;
        } a5; // Add Imm22
        struct {
            uint64_t qp:6;
            uint64_t p1:6;
            uint64_t c:1;
            uint64_t r2:7;
            uint64_t r3:7;
            uint64_t p2:6;
            uint64_t ta:1;
            uint64_t x2:2;
            uint64_t tb:1;
            uint64_t constCtoE:4;
        } a6; // Compare
        struct {
            uint64_t qp:6;
            uint64_t p1:6;
            uint64_t c:1;
            uint64_t imm7b:7;
            uint64_t r3:7;
            uint64_t p2:6;
            uint64_t ta:1;
            uint64_t x2:2;
            uint64_t s:1;
            uint64_t constCtoE:4;
        } a8; // Compare Imm8


        // I //
        struct {
            uint64_t qp:6;
            uint64_t r1:7;
            uint64_t y:1;
            uint64_t pos6b:6;
            uint64_t r3:7;
            uint64_t len6d:6;
            uint64_t x:1;
            uint64_t x2:2;
            uint64_t:1;
            uint64_t const5:5;
        } i11; // Extract
        struct {
            uint64_t qp:6;
            uint64_t r1:7;
            uint64_t:1;
            uint64_t cpos6b:6;
            uint64_t r3:7;
            uint64_t len6d:6;
            uint64_t x:1;
            uint64_t x2:2;
            uint64_t s:1;
            uint64_t const5:4;
        } i14; // Deposit Imm1
        struct {
            uint64_t qp:6;
            uint64_t r1:7;
            uint64_t r2:7;
            uint64_t r3:7;
            uint64_t len4d:4;
            uint64_t cpos6d:6;
            uint64_t const4:4;
        } i15; // Deposit
        struct {
            uint64_t qp:6;
            uint64_t p1:6;
            uint64_t c:1;
            uint64_t y:1;
            uint64_t pos6b:6;
            uint64_t r3:7;
            uint64_t p2:6;
            uint64_t ta:1;
            uint64_t x2:2;
            uint64_t tb:1;
            uint64_t const5:4;
        } i16; // Test Bit
        struct {
            uint64_t qp:6;
            uint64_t imm20a:20;
            uint64_t:1;
            uint64_t x6:6;
            uint64_t x3:3;
            uint64_t i:1;
            uint64_t const0:4;
        } i19; // Break/Nop
        struct {
            uint64_t qp:6;
            uint64_t b1:3;
            uint64_t:4;
            uint64_t r2:7;
            uint64_t wh:2;
            uint64_t x:1;
            uint64_t ih:1;
            uint64_t timm9c:9;
            uint64_t x3:3;
            uint64_t:1;
            uint64_t const0:4;
        } i21; // Move to BR
        struct {
            uint64_t qp:6;
            uint64_t r1:7;
            uint64_t b2:3;
            uint64_t:11;
            uint64_t x6:6;
            uint64_t x3:3;
            uint64_t:1;
            uint64_t const0:4;
        } i22; // Move from BR
        struct {
            uint64_t qp:6;
            uint64_t r1:7;
            uint64_t:14;
            uint64_t x6:6;
            uint64_t x3:3;
            uint64_t:1;
            uint64_t const0:4;
        } i25; // Move from Pred/IP
        struct {
            uint64_t qp:6;
            uint64_t:7;
            uint64_t r2:7;
            uint64_t ar3:7;
            uint64_t x6:6;
            uint64_t x3:3;
            uint64_t:1;
            uint64_t const0:4;
        } i26; // Move to AR
        struct {
            uint64_t qp:6;
            uint64_t:7;
            uint64_t imm7b:7;
            uint64_t ar3:7;
            uint64_t x6:6;
            uint64_t x3:3;
            uint64_t s:1;
            uint64_t const0:4;
        } i27; // Move to AR Imm8
        struct {
            uint64_t qp:6;
            uint64_t r1:7;
            uint64_t:7;
            uint64_t ar3:7;
            uint64_t x6:6;
            uint64_t x3:3;
            uint64_t:1;
            uint64_t const0:4;
        } i28; // Move from AR


        // M //
        struct {
            uint64_t qp:6;
            uint64_t:21;
            uint64_t x4:4;
            uint64_t x2:2;
            uint64_t x3:3;
            uint64_t:1;
            uint64_t const0:4;
        } m24; // Sync/Srlz/ALAT
        struct {
            uint64_t qp:6;
            uint64_t:7;
            uint64_t r2:7;
            uint64_t ar3:7;
            uint64_t x6:6;
            uint64_t x3:3;
            uint64_t:1;
            uint64_t const1:4;
        } m29; // Move to AR
        struct {
            uint64_t qp:6;
            uint64_t:7;
            uint64_t r2:7;
            uint64_t cr3:7;
            uint64_t x6:6;
            uint64_t x3:3;
            uint64_t:1;
            uint64_t const1:4;
        } m32; // Move to CR
        struct {
            uint64_t qp:6;
            uint64_t r1:7;
            uint64_t:7;
            uint64_t cr3:7;
            uint64_t x6:6;
            uint64_t x3:3;
            uint64_t:1;
            uint64_t const1:4;
        } m33; // Move from CR
        struct {
            uint64_t qp:6;
            uint64_t r1:7;
            uint64_t:14;
            uint64_t x6:6;
            uint64_t x3:3;
            uint64_t:1;
            uint64_t const1:4;
        } m36; // Move from PSR
        struct {
            uint64_t qp:6;
            uint64_t imm20a:20;
            uint64_t:1;
            uint64_t x4:4;
            uint64_t x2:2;
            uint64_t x3:3;
            uint64_t i:1;
            uint64_t const0:4;
        } m37; // Break/Nop
        struct {
            uint64_t qp:6;
            uint64_t:7;
            uint64_t r2:7;
            uint64_t r3:7;
            uint64_t x6:6;
            uint64_t x3:3;
            uint64_t:1;
            uint64_t const1:4;
        } m42; // Mv to Ind/TR Ins
        struct {
            uint64_t qp:6;
            uint64_t r1:7;
            uint64_t:7;
            uint64_t r3:7;
            uint64_t x6:6;
            uint64_t x3:3;
            uint64_t:1;
            uint64_t const1:4;
        } m43; // Mv from Ind
        struct {
            uint64_t qp:6;
            uint64_t imm21a:21;
            uint64_t x4:4;
            uint64_t i2d:2;
            uint64_t x3:3;
            uint64_t i:1;
            uint64_t const0:4;
        } m44; // Set/Reset Mask


        // B // 
        struct {
            uint64_t qp:6;
            uint64_t btype:3;
            uint64_t:3;
            uint64_t p:1;
            uint64_t imm20b:20;
            uint64_t wh:2;
            uint64_t d:1;
            uint64_t s:1;
            uint64_t const4:4;
        } b1; // IP-Relative Branch
        struct {
            uint64_t qp:6;
            uint64_t btype:3;
            uint64_t:3;
            uint64_t p:1;
            uint64_t imm20b:20;
            uint64_t wh:2;
            uint64_t d:1;
            uint64_t s:1;
            uint64_t const4:4;
        } b2; // Counted Branch
        struct {
            uint64_t qp:6;
            uint64_t b1:3;
            uint64_t:3;
            uint64_t p:1;
            uint64_t imm20b:20;
            uint64_t wh:2;
            uint64_t d:1;
            uint64_t s:1;
            uint64_t const5:4;
        } b3; // IP-Relative Call
        struct {
            uint64_t qp:6; // 0
            uint64_t:21;
            uint64_t x6:6;
            uint64_t:4;
            uint64_t const0:4;
        } b8; // Misc
        struct {
            uint64_t qp:6;
            uint64_t imm20a:20;
            uint64_t:1;
            uint64_t x6:6;
            uint64_t:3;
            uint64_t i:1;
            uint64_t const02:4;
        } b9; // Break/Nop


        // F //
        struct {
            uint64_t qp:6;
            uint64_t imm20a:20;
            uint64_t:1;
            uint64_t x6:6;
            uint64_t x3:3;
            uint64_t i:1;
            uint64_t const0:4;
        } f15; // Break/Nop


        // X //
        struct {
            uint64_t qp:6;
            uint64_t r1:7;
            uint64_t imm7b:7;
            uint64_t vc:1;
            uint64_t ic:1;
            uint64_t imm5c:5;
            uint64_t imm9d:9;
            uint64_t i:1;
            uint64_t const6:4;
        } x2; // Move Imm64

        uint64_t raw;
    };

    Ia64Format(Ia64Bundle *bundle, unsigned int slot) {
        switch (slot) {
            default: {
                fprintf(stderr, "slot %d doesn't exist!!\n", slot);
                exit(1);
                break;
            }
            case 0: {
                raw = bundle->_instslot0;
                break;
            }
            case 1: {
                raw = bundle->_instslot1;
                break;
            }
            case 2: {
                raw = bundle->_instslot2;
                break;
            }
        }
    }
};
#pragma pack(pop)

#define DECLINST(name) static void name(Ia64Format *format, Ia64Cpu *cpu)
