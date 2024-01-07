#pragma once
#include "memory.hpp"
#include "fp.hpp"
#include "bundle.hpp"
#include "pe.hpp"

class Ia64Regs {
public:
    // there is a better way to do this...
    struct Gpr {
        #define GPR_ZEROREG 0
        struct GprVal {
            uint64_t val;
            bool nat = false;
            int id;
            uint64_t operator=(uint64_t v) {
                if (id == GPR_ZEROREG) {
                    printf("trying to write zero gpr\n");
                    return 0;
                }
                return (val = v);
            }
        } _val[128];
        GprVal &operator[](int idx) {
            _val[idx].id = idx;
            return _val[idx];
        }
    } gpr; // general reg
    #define FPR_ZEROREG 0
    #define FPR_ONEREG  1
    struct Fpr {
        struct FprVal : Ia64Float {
            int id;
            Ia64Float operator=(Ia64Float f) {
                if (id == FPR_ZEROREG) {
                    printf("trying to write zero fpr\n");
                    return Ia64Float(0);
                }
                if (id == FPR_ONEREG) {
                    printf("trying to write one fpr\n");
                    return Ia64Float(1);
                }
                return Ia64Float::operator=(f);
            }
        } _val[128];
        FprVal &operator[](int idx) {
            _val[idx].id = idx;
            return _val[idx];
        }
    } fpr; // floating reg
    struct Pr {
        #define PR_ONEREG 0
        struct PrVal {
            bool val;
            int id;
            bool operator=(bool v) {
                if (id == PR_ONEREG) {
                    printf("trying to write one pr\n");
                    return 1;
                }
                return (val = v);
            }
        } _val[64];
        PrVal &operator[](int idx) {
            _val[idx].id = idx;
            return _val[idx];
        }
    } pr; // predicate reg

    struct Br {
        uint64_t val;
        uint64_t operator=(uint64_t v) {
            return (val = v);
        }
    } br[8]; // branch reg

    struct Ar {
        uint64_t val;
        uint64_t operator=(uint64_t v) {
            return (val = v);
        }

        enum Type {
            /* 
            KR = (0, 1, 2, 3, 4, 5, 6, 7) # Kernel Registers
RSC = 16        # Register Stack Configuration Register
BSP = 17        # Backing Store Pointer (read-only)
BSPSTORE = 18   # Backing Store Pointer for Memory Stores
RNAT = 19       # RSE NAT Collection Register
FCR = 21        # IA-32 Floating-point Control Register
EFLAG = 24      # IA-32 EFLAG register
CSD = 25        # IA-32 Code Segment Descriptor
SSD = 26        # IA-32 Stack Segment Descriptor
CFLG = 27       # IA-32 Combined CR0 and CR4 register
FSR = 28        # IA-32 Floating-point Status Register
FIR = 29        # IA-32 Floating-point Instruction Register
FDR = 30        # IA-32 Floating-point Data Register
CCV = 32        # Compare and Exchange Compare Value Register
UNAT = 36       # User NAT Collection Register
FPSR = 40       # Floating-Point Status Register
ITC = 44        # Interval Time Counter
PFS = 64        # Previous Function State
LC = 65         # Loop Count Register
EC = 66         # Epilog Count Register
            */
            KR0 = 0,
            KR1 = 1,
            KR2 = 2,
            KR3 = 3,
            KR4 = 4,
            KR5 = 5,
            KR6 = 6,
            KR7 = 7,
            RSC = 16,
            BSP = 17,
            BSPSTORE = 18,
            RNAT = 19,
            FCR = 21,
            EFLAG = 24,
            CSD = 25,
            SSD = 26,
            CFLG = 27,
            FSR = 28,
            FIR = 29,
            FDR = 30,
            CCV = 32,
            UNAT = 35,
            FPSR = 40,
            ITC = 44,
            PFS = 64,
            LC = 65,
            EC = 66,

        };

        void SetPFS(uint64_t pfm, uint64_t pec, uint64_t ppl) {
            val = (pfm) | (pec << 52) | (ppl << 62);
        }

    } ar[128]; // application reg

    uint64_t ip; // instruction pointer

    #pragma pack(push,1)
    struct Cfm {
        struct Rrb {
            uint32_t gr:7; // general reg
            uint32_t fr:7; // float reg
            uint32_t pr:6; // predicate reg
           // Rrb() {
            //    gr = 0;
            //    fr = 0;
            //    pr = 0;
            //}
        }; // register rename base
        union {
            struct {
                uint32_t sof:7; // stack frame size
                uint32_t sol:7; // size of locals portion of stack frame
                uint32_t sor:4; // size of rotating portion of stack frame
                Rrb rrb;
            };
            uint64_t raw;
        };
    Cfm() {
        sof = 0;
        sol = 0;
        sor = 0;
        rrb.gr = 0;
        rrb.fr = 0;
        rrb.pr = 0;
    }
    } cfm; // current frame marker

    struct Psr {
        union {
            struct {
                uint64_t rv0:1; // reserved
                uint64_t be:1; // big endian
                uint64_t up:1; // user perfomance monitor enable
                uint64_t ac:1; // alignment check
                uint64_t mfl:1; // lower FP (2-31)regs written
                uint64_t mfh:1; // upper FP (32-127)regs written
                uint64_t rv1:7; // reserved
                uint64_t ic:1; // interruption collection
                uint64_t i:1; // interrupt bit
                uint64_t pk:1; // protection key enable
                uint64_t rv2:1; // reserved
                uint64_t dt:1; // data addr translation
                uint64_t dfl:1; // disabled FP low reg set
                uint64_t dfh:1; // disable FP high reg set
                uint64_t sp:1; // secure perfomance monitores
                uint64_t pp:1; // privileged perfomance monitor enable
                uint64_t di:1; // disable instruction set transition
                uint64_t si:1; // secure interval timer
                uint64_t db:1; // debug breakpoint fault
                uint64_t lp:1; // lower privilege transfer trap
                uint64_t tb:1; // taken branch trap
                uint64_t rt:1; // register stack translation
                uint64_t rv3:4; // reserved
                uint64_t cpl:2; // current privilege level
                uint64_t is:1; // instruction set
                uint64_t mc:1; // machine check abort mask
                uint64_t it:1; // instruction address translation
                uint64_t id:1; // instruction debug fault disable
                uint64_t da:1; // disable data access/dirty-bit faults
                uint64_t dd:1; // data debug fault disable
                uint64_t ss:1; // single step enable
                uint64_t ri:2; // restart instruction
                uint64_t ed:1; // exception deferral
                uint64_t bn:1; // register bank
                uint64_t ia:1; // disable inst access-bit faults
                uint64_t rv4:18; // reserved
            };
            uint64_t raw;
        };
    } psr; // processor status register
    #pragma pack(pop)

    Ia64Regs() {
        gpr[GPR_ZEROREG] = 0;
        fpr[FPR_ZEROREG] = 0;
        fpr[FPR_ONEREG] = 1;
        pr[PR_ONEREG] = 1;
        ip = 0; // TODO: proper bootloader
        for (int i = 1; i < NELEM(gpr); i++) {
            gpr[i] = 0;
        }
        for (int i = 2; i < NELEM(fpr); i++) {
            fpr[i] = 0;
        }
        for (int i = 1; i < NELEM(pr); i++) {
            pr[i] = 0;
        }
        for (int i = 0; i < NELEM(br); i++) {
            br[i] = 0;
        }
        for (int i = 0; i < NELEM(ar); i++) {
            ar[i] = 0;
        }
    }

    void CheckTargetRegister(uint64_t r) {
        debugprintf("TODO: check_target_register(%ld)\n", r);
        // If r1 targets an out-of-frame stacked register (as defined by CFM), an illegal operation 
        // fault is delivered, and this function does not return.
        // whatever ^^^^ is: TODO
    }
    bool GetPR(uint8_t r) {
        if (r == PR_ONEREG) {
            return true;
        }
        return pr[r].val;
    }
};

class Ia64Alat { // Advanced load address table
public:
    void FrameUpdate(int a, int b) {
        debugprintf("TODO: alat_frame_update(%d, %d)\n", a, b);
    }
};

class Ia64Rse { // Register stack engine
public:
    void NewFrame(int a, int b) {
        debugprintf("TODO: rse_new_frame(%d, %d)\n", a, b);
    }
    void PreserveFrame(int a) {
        debugprintf("TODO: rse_preserve_frame(%d)\n", a);
    }
};

class Ia64Cpu {
public:
    Ia64Cpu(uint64_t ib, uint64_t entry) {
        imageBase = ib;
        //regs.ip = entry + ib;
        //printf("%lx\n", entry-PE::GetNt()->OptionalHeader.SizeOfHeaders-PE::GetNt()->OptionalHeader.SectionAlignment);
        PE::ReadAt<uint64_t>(&regs.ip, entry - PE::GetNt()->OptionalHeader.SizeOfHeaders - PE::GetNt()->OptionalHeader.SectionAlignment); // load func ptr
        PE::ReadAt<uint64_t>(&regs.gpr[1].val, entry - PE::GetNt()->OptionalHeader.SizeOfHeaders - PE::GetNt()->OptionalHeader.SectionAlignment + 8); // load gp
        halt = false;
        branched = false;
    } 

    ~Ia64Cpu() {}

    void run(void);
    void Dump(void);

    Ia64Regs regs;
    Ia64Alat alat;
    Ia64Rse rse;
    uint64_t imageBase;
    bool halt;
    bool branched;

    // checks //

    bool UnimplementedVirtualAddress(uint64_t ip) {
        debugprintf("TODO: unimplemented_virtual_address\n");
        return false;
    }

    // faults //

    void IllegalOperationFault(void) {
        debugprintf("Got an illegal operation fault!!\n");
    }

    void ReservedRegisterFieldFault(void) {
        debugprintf("Got a reserved register field fault!!!\n");
    }

    void RegisterNatConsumptionFault(uint8_t a) {
        debugprintf("Got a register NaT consumption fault with %d\n", a);
    }


    // traps //
    
    void UnimplementedInstructionAddressTrap(int a, uint64_t ip) {
        debugprintf("Got a unimplemented_instruction_address_trap!! %lx %lx\n", a, ip);
    }

    void TakenBranchTrap(void) {
        debugprintf("Got a taken_branch_trap!!\n");
    }

    ////////////
};