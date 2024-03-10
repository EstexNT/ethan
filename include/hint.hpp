#pragma once

#include "common.hpp"

namespace Hint {

/* HINT TYPES: 
    -Load Hints
        0 = None; Temporal locality, level 1
        1 = nt1; No temporal locality, level 1
        2 = nta; No temporal locality, all levels
    -TODO

*/

static inline const char *GetHintStr(uint8_t hint) {
    if (hint > 3) return "";
    static char *hintStrTable[] = {

    };
    return (const char *[]){"", ".nt1", ".bad", ".nta"}[hint];
}

} // Hint
