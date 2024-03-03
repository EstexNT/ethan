#pragma once

#include "common.h"
#include "pe.hpp"


namespace Memory {

#define MEMLIST_MEMSIZE 0x200

struct MemoryList {
    void *memptr;
    Ia64Addr addr;


    MemoryList(void) {
        memptr = nullptr;
        addr = 0;
    }
    MemoryList(Ia64Addr argaddr) {
        memptr = malloc(MEMLIST_MEMSIZE);
        addr = argaddr;
    }

    bool InRange(Ia64Addr argaddr) {
        return ((argaddr >= addr) && ((addr + MEMLIST_MEMSIZE) > argaddr));
    }

    size_t Diff(Ia64Addr argaddr) {
        return abs(long(argaddr - addr));
    }

    template<typename T> 
    void ReadAt(T *buf, Ia64Addr argaddr) {
        if (!InRange(argaddr)) {
            fprintf(stderr, "trying to read at %lx which is out of range for %lx\n", argaddr, addr);
            return;
        }
        memcpy(buf, (uint8_t *)memptr + Diff(argaddr), sizeof(T));
    }
    template<typename T>
    void WriteAt(T *src, Ia64Addr argaddr) {
        if (!InRange(argaddr)) {
            fprintf(stderr, "trying to write to %lx which is not in range of %lx\n", argaddr, addr);
            return;
        }
        memcpy((uint8_t *)memptr + Diff(argaddr), src, sizeof(T));
    }
};

static inline MemoryList *MakeList(void) {
    MemoryList *mlist = new MemoryList;
    if (mlist == nullptr) {
        fprintf(stderr, "Couldn't make a list!\n");
        exit(1);
        return nullptr;
    }
    return mlist;
}

static inline MemoryList *MakeList(Ia64Addr addr) {
    MemoryList *mlist = new MemoryList(addr);
    if (mlist == nullptr) {
        fprintf(stderr, "Couldn't make a list!\n");
        exit(1);
        return nullptr;
    }
    return mlist;
}

extern bool Init(void);
extern void Exit(void);
extern MemoryList *GetList(Ia64Addr addr);
template<typename T>
static inline void ReadAt(T *buf, Ia64Addr addr) {
    GetList(addr)->ReadAt<T>(buf, addr);
}
template<typename T>
static inline void WriteAt(T *buf, Ia64Addr addr) {
    GetList(addr)->WriteAt<T>(buf, addr);
}
static inline char *ReadString(Ia64Addr addr) {
    char *outstr = (char *)malloc(1);
    char tempchr = '\0';
    int i = 0;
    do {
        ReadAt<char>(&tempchr, addr + i);
        outstr[i] = tempchr;
        i++;
        outstr = (char *)realloc(outstr, 1 + i);
    } while (tempchr != '\0');
    return outstr;
}

static inline void LoadPE(void) {
    static unsigned char tempbuf[MEMLIST_MEMSIZE];
    uint64_t imagebase = PE::GetNt()->OptionalHeader.ImageBase;
    uint64_t imageend = imagebase + PE::GetSize();

    for (Ia64Addr addr = imagebase; addr < imageend; addr += MEMLIST_MEMSIZE) {
        PE::ReadAt<unsigned char[MEMLIST_MEMSIZE]>(&tempbuf, GetPhysAddr(addr));
        GetList(addr)->WriteAt<unsigned char[MEMLIST_MEMSIZE]>(&tempbuf, addr);
    }
}

} //namespace Memory
