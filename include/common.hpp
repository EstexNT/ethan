#pragma once

//#define DEBUGPRINT

#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <cinttypes>

#define NELEM(a) ((size_t) (sizeof((a)) / (sizeof((a)[0]))))

typedef uint64_t Ia64Addr;

static inline uint64_t SignExt(uint64_t value, uint8_t pos) {
    uint64_t out = value;
    if (pos >= 64) {
        return out;
    }
    if (value >= (1 << (pos - 1))) {
        out = -((1 << (pos)) - value);
    }
    return out;
}

static inline uint64_t uint_pow(uint64_t base, uint64_t exp) {
    uint64_t result = 1;

    while (exp) {
        if (exp % 2) {
           result *= base;
        }
        exp /= 2;
        base *= base;
    }
    return result;
}

static inline uint64_t ZeroExt(uint64_t value, uint8_t pos) {
    uint64_t out = value;
    if (pos >= 64) {
        return out;
    }
    uint64_t mask = uint_pow(pos, 2) - 1;
    return out & mask;
}

static inline void debugprintf(const char *fmt, ...) {
#ifdef DEBUGPRINT
    va_list list;
    va_start(list, fmt);
    vprintf(fmt, list);
    va_end(list);
#endif
}
