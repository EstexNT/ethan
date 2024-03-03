#include "common.h"
#include "pe.hpp"
#include "memory.hpp"


namespace MSVCR80 {



static struct {
    char *name;
    void *ptr;
} funcNames[] = {
    { "__initenv", nullptr },
};



} // namespace MSVCR80

ResolvedImport ResolveMSVCR80(char *name) {
    debugprintf("resolving %s\n", name);
    for (int i = 0; i < NELEM(MSVCR80::funcNames); i++) {
        if (strcmp(name, MSVCR80::funcNames[i].name) == 0) {
            return { MakeUpImportAddr("MSVCR80", MSVCR80::funcNames[i].name), MSVCR80::funcNames[i].ptr };
        }
    }
    return {0, nullptr};
}
