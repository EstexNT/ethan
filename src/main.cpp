#include "cpu.hpp"
#include "memory.hpp"

void printusage(char *progname) {
    printf("Usage: \n");
    printf("%s <bios file>\n", progname);
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

    if (!Memory::LoadBIOS(argv[1])) {
        fprintf(stderr, "Couldn't load the BIOS!\n");
        Memory::Exit();
        return 1;
    }

    
    Ia64Cpu cpu;
    cpu.run();

    Memory::Exit();

    return 0;
}
