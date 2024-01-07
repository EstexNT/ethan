#include "memory.hpp"

namespace Memory {

static MemoryList **memlist = nullptr;
static int listcount = 0;

bool Init(void) {
    memlist = (MemoryList **)malloc(sizeof(MemoryList *) * (listcount + 1));

    if (memlist == nullptr) {
        perror("Couldn't create memory list");
        exit(1);
        return false;
    }

    memlist[listcount++] = MakeList();

    return true;
}

void Exit(void) {
    for (int i = 0; i < listcount; i++) {
        if (memlist[i]) {
            if (memlist[i]->memptr) {
                free(memlist[i]->memptr);
            }
            delete memlist[i];
        }
    }
    listcount = 0;
    free(memlist);
}

MemoryList *GetList(Ia64Addr addr) {
    for (int i = 0; i < listcount; i++) {
        if (memlist[i]->InRange(addr)) {
            return memlist[i];
        }
    }
    // list wasn't found, create it
    memlist = (MemoryList **)realloc(memlist, sizeof(MemoryList *) * (listcount + 1));
    
    return (memlist[listcount++] = MakeList(addr));
}


} // namespace Memory
