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


        // I //
        struct {
            uint64_t qp:6;
            uint64_t imm20a:20;
            uint64_t:1;
            uint64_t x6:6;
            uint64_t x3:3;
            uint64_t i:1;
            uint64_t const0:4;
        } i19; // Break/Nop


        // M //
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
            uint64_t qp:6;
            uint64_t imm20a:20;
            uint64_t:1;
            uint64_t x6:6;
            uint64_t:3;
            uint64_t i:1;
            uint64_t const02:4;
        } b9; // Break/Nop


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
