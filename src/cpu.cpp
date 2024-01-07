#include "cpu.hpp"

static uint8_t stackBuffer[64*1024] = {0};

static char logBuf[0x10000] = {0};

void Ia64Cpu::Dump(void) {
#define lb (logBuf+strlen(logBuf))
    FILE *outlog = fopen("cpu.log", "w+");

    sprintf(logBuf, 
    "\n=== CPU DUMP START ===\n"
    "\tRegisters:\n"
    "\t\tGPR:\n\t\t  ");
    for (int i = 0; i < NELEM(regs.gpr); i++) {
        sprintf(lb, 
            "GPR%d = 0x%016" PRIx64 ", ", i, regs.gpr[i].val
        );
        if ((i % 7) == 0) {
            sprintf(lb, "\n\t\t  ");
        }
    }

    sprintf(lb, "\n\t\tFPR:\n\t\t  ");

    for (int i = 0; i < NELEM(regs.fpr); i++) {
        sprintf(lb,  // TODO: fix this
            "FPR%d = 0x%016" PRIx64 " %016" PRIx64 ", ", i, 
                    uint64_t(regs.fpr[i].val >> 64), 
                        uint64_t(regs.fpr[i].val));
        if (i == 1) {
            sprintf(lb, "\n\t\t  ");
        }
        if ((i % 7) == 0) {
            sprintf(lb, "\n\t\t  ");
        }
    }
    sprintf(lb, "\n\t\tPR:\n\t\t  ");
    for (int i = 0; i < NELEM(regs.pr); i++) {
        sprintf(lb, 
            "PR%d = 0x%02x, ", i, regs.pr[i].val);
        if ((i % 16) == 0) {
            sprintf(lb, "\n\t\t  ");
        }
    }
    sprintf(lb, "\n\t\tBR:\n\t\t  ");
    for (int i = 0; i < NELEM(regs.br); i++) {
        sprintf(lb,
            "BR%d = 0x%016" PRIx64 ", ", i, regs.br[i].val);
    }
    sprintf(lb, "\n\t\tAR:\n\t\t  ");
    for (int i = 0; i < NELEM(regs.ar); i++) {
        sprintf(lb, 
            "AR%d = 0x%016" PRIx64 ", ", i, regs.ar[i].val);
        if ((i % 7) == 0) {
            sprintf(lb, "\n\t\t  ");
        }
    }

    sprintf(lb, "\n\t\tIP:\n\t\t 0x%016" PRIx64 "\n", regs.ip);

    sprintf(lb,   "\t\tCFM:\n\t\t 0x%016" PRIx64 "\n", regs.cfm.raw);

    sprintf(lb,   "\t\tPSR:\n\t\t 0x%016" PRIx64 "\n", regs.psr.raw);

    sprintf(lb, "\n=== CPU DUMP END ===\n");

    fwrite(logBuf, strlen(logBuf), 1, outlog);

    fclose(outlog);
#undef lb
}
#define STACK_VALUE 0xfffe0000 
#define STACK_SIZE (64 * 1024)
void Ia64Cpu::run(void) {

    // TODO:
    // write a bootloader so that the stack is set up properly (instead of in emu)
    regs.gpr[12] = uint64_t(STACK_VALUE + STACK_SIZE - 128);
    Ia64Bundle bundle {0};

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
        
        if (regs.gpr[12].val <= STACK_VALUE) {
            fprintf(stderr," !!!! STACK BUFFER OVERRUN !!!!");
            return;
        }

        uint64_t theip = GetPhysAddr(regs.ip);
        printf("%lx:\t", regs.ip);
        PE::ReadAt<uint128_t>(&bundle.raw, theip);
        debugprintf("bundle: %016" PRIx64 " %016" PRIx64 "\n", uint64_t((bundle.raw >> 64) & UINT64_MAX), uint64_t(bundle.raw & UINT64_MAX));
        bundle.Handle(this);
        if (!branched) {
            regs.ip += sizeof(uint128_t); 
        } else {
            branched = false; // branches don't increment ip (I think)
        }
    }
}