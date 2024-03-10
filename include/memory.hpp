#pragma once

#include "common.h"


namespace Memory {

enum MemoryAddresses {
    BIOS_ROM = 0xffc00000,
};


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

static inline bool LoadBIOS(char *biosFileName) {
    FILE *biosf = fopen(biosFileName, "rb");
    if (biosf == nullptr) {
        perror("Couldn't open the BIOS file");
        return false;
    }

    int i = 0;
    static uint8_t tempbuf[MEMLIST_MEMSIZE] = {0};
    printf("Reading the BIOS...");
    while (i != ((0x100000000 - BIOS_ROM))) {
        fread(tempbuf, MEMLIST_MEMSIZE, 1, biosf);
        WriteAt<uint8_t[MEMLIST_MEMSIZE]>(&tempbuf, BIOS_ROM + i);
        i += MEMLIST_MEMSIZE;
    }
    printf(" Done.\n");
    fclose(biosf);
    return true;
}

} //namespace Memory
