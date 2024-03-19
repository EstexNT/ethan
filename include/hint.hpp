#pragma once

#include "common.hpp"

namespace Hint {


static inline const char *GetHintStr(uint8_t hint) {
    if (hint > 3) {
        return "";
    }
    return (const char *[]){"", ".nt1", ".bad", ".nta"}[hint];
}
static inline const char *GetBranchImportanceHintStr(uint8_t ih) {
    if (ih > 1) {
        return "";
    }
    return (const char *[]){"", ".imp"}[ih];
}


} // Hint
