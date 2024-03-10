#pragma once
#include "common.hpp"

class Ia64Float {
public:
    // TODO: implement this yeah
    uint64_t val[2];

    Ia64Float(uint64_t n) {
        val[0] = n;
        val[1] = 0;
    }

    Ia64Float(uint64_t n, uint64_t n1) {
        val[0] = n;
        val[1] = n1;
    }
    Ia64Float operator=(Ia64Float f) {
        val[0] = f.val[0];
        val[1] = f.val[1];
        return *this;
    }

    Ia64Float() {
        val[0] = val[1] = 0;
    }
};
