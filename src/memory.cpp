#include <list>
#include "memory.hpp"

namespace Memory {


static std::list<MemoryList *> memlist {};

bool Init(void) {
    // TODO: do things here
    return true;
}

void Exit(void) {
    for (auto mlist : memlist) {
        if (mlist) {
            if (mlist->memptr) {
                delete mlist->memptr;
            }
            delete mlist;
        }
    }
    memlist.clear();
}

MemoryList *GetList(Ia64Addr addr) {
    for (auto mlist : memlist) {
        if (mlist->InRange(addr)) {
            return mlist;
        }
    }

    // list wasn't found, create it
    auto outlist = MakeList(addr);
    memlist.push_back(outlist);
    return outlist;
}


} // namespace Memory
