#include <fstream>
#include <iomanip>
#include <array>
#include "cpu.hpp"


void Ia64Cpu::Dump(void) {
#define TOHEX16 std::hex << std::setw(16) << std::setfill('0')
#define TOHEX2 std::hex << std::setw(2) << std::setfill('0')
#define TODEC std::dec << std::setw(0) << std::setfill(' ')

    std::ofstream outlog("cpu.log", std::ios::out);
    outlog << "\n=== CPU DUMP START ===\n";
    outlog << "\tRegisters:\n";
    
    outlog << "\t\tGPR:\n\t\t  ";
    for (int i = 0; i < NELEM(regs.gpr); i++) {
        outlog << "GPR" << TODEC << i;
        outlog << " = 0x" << TOHEX16 << regs.gpr[i].val << ", ";
        if ((i % 7) == 0) {
            outlog << "\n\t\t  ";
        }
    }

    outlog << "\n\t\tFPR:\n\t\t  ";
    for (int i = 0; i < NELEM(regs.fpr); i++) {
        outlog << "FPR" << TODEC << i;
        outlog << " = 0x" << TOHEX16 << regs.fpr[i].val.val[1] << TOHEX16 << regs.fpr[i].val.val[0] << ", ";
        if (i == 1) {
            outlog << "\n\t\t  ";
        }
        if ((i % 7) == 0) {
            outlog << "\n\t\t  ";
        }
    }    

    outlog << "\n\t\tPR:\n\t\t  ";
    for (int i = 0; i < NELEM(regs.pr); i++) {
        outlog << "PR" << TODEC << i;
        outlog << " = 0x" << TOHEX2 << regs.pr[i].val << ", ";
        if ((i % 16) == 0) {
            outlog << "\n\t\t  ";
        }
    }

    outlog << "\n\t\tBR:\n\t\t  ";
    for (int i = 0; i < NELEM(regs.br); i++) {
        outlog << "BR" << TODEC << i;
        outlog << " = 0x" << TOHEX16 << regs.br[i].val << ", ";
    }

    outlog << "\n\t\tAR:\n\t\t  ";
    for (int i = 0; i < NELEM(regs.ar); i++) {
        outlog << "AR" << TODEC << i;
        outlog << " = 0x" << TOHEX16 << regs.ar[i].val << ", ";
        if ((i % 7) == 0) {
            outlog << "\n\t\t  ";
        }
    }

    outlog << "\n\t\tIP:\n\t\t 0x" << TOHEX16 << regs.ip << "\n";
    outlog << "\t\tCFM:\n\t\t 0x" << TOHEX16 << regs.cfm.raw << "\n";
    outlog << "\t\tPSR:\n\t\t 0x" << TOHEX16 << regs.psr.raw << "\n";

    outlog << "\t\tCPUID:\n\t\t  ";
    for (int i = 0; i < (regs.cpuid[3].val & 0xff); i++) {
        outlog << "CPUID" << TODEC << i;
        outlog << " = 0x" << TOHEX16 << regs.cpuid[i].val << ", ";
    }
    outlog << "\n";
    outlog << "\n=== CPU DUMP END ===\n";

    outlog.close();
}

void Ia64Cpu::run(void) {
    Ia64Bundle bundle {};

    while (true) {
        if (halt) {
            printf("got a halt!\n");
            printf("dump the CPU state?\n>");
            int reply = getchar();
            if ((reply == 'y') || (reply == '\n')) {
                printf("\nOK\n");
                Dump();
                printf("Done\n");
                return;
            }
            return;
        }
        Memory::ReadAt<Ia64Bundle>(&bundle, regs.ip);
        printf("%lx:\t", regs.ip);
        debugprintf("bundle: %016" PRIx64 " %016" PRIx64 "\n", bundle.raw[1], bundle.raw[0]);
        auto handled = bundle.Handle(this);
        for (unsigned int i = 0; i < handled.size(); i++) {
            if (branched) {
                printf("(no inst; branched!)\n");
            } else {
                Ia64Format format { &bundle, i };
                handled[i](&format, this);
            }
        }
        if (!branched) {
            regs.ip += IA64BUNDLESIZE; 
        } else {
            branched = false;
        }
    }
}
