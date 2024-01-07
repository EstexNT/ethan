#pragma once
#include <stdint.h>
#include "n128.h"

class Ia64Float {
public:
    // TODO: implement this yeah
    uint128_t val;

    Ia64Float(uint128_t n) {
        val = n;
    }
    Ia64Float operator=(Ia64Float f) {
        val = f.val;
    }

    Ia64Float() {
        val = 0;
    }
};