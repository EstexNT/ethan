#include <stdio.h>
#include <stdint.h>
#include "pe.hpp"
#include "cpu.hpp"
#include "memory.hpp"

void printusage(char *progname) {
    printf("Usage: \n");
    printf("%s <.exe file>\n", progname);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printusage(argv[0]);
        return 1;
    }

    if (!Memory::Init()) {
        fprintf(stderr, "Couldn't initialize memory!");
        return 1;
    }

    if (!PE::Open(argv[1])) {
        fprintf(stderr, "Couldn't open %s!\n", argv[1]);
        perror(argv[1]);
        return 1;
    }


    IMAGE_DOS_HEADER *dosHead = PE::ParseDos();
    IMAGE_NT_HEADERS64 *ntHead = PE::ParseNt();

    if (ntHead->FileHeader.Machine != IMAGE_FILE_MACHINE_IA64) {
        fprintf(stderr, "%s is not an IA-64 PE file!\n", argv[1]);
        PE::Close();
        return 1;
    }

    IMAGE_SECTION_HEADER *sections = PE::ParseSections();
    debugprintf("image base: %016" PRIx64 "; entry: %08x\n", ntHead->OptionalHeader.ImageBase, ntHead->OptionalHeader.AddressOfEntryPoint);
    Memory::LoadPE();
    
    Ia64Cpu cpu {ntHead->OptionalHeader.ImageBase, ntHead->OptionalHeader.AddressOfEntryPoint};
    cpu.run();

    PE::Close();
    Memory::Exit();

    return 0;
}
