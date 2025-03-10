#pragma once
#include "common.hpp"
#include "memory.hpp"
#include "fp.hpp"
#include "format.hpp"

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
                    exit(1); // TODO: should fault
                    return 0;
                }
                return (val = v);
            }
            bool operator=(bool b) {
                if (id == GPR_ZEROREG) {
                    printf("trying to write nat to zero gpr\n");
                    exit(1); // TODO: should fault
                    return 0;
                }
                return (nat = b);
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
        struct FprVal {
            Ia64Float val;
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
                return (val = f);
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
                    //printf("trying to write one pr\n");
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

    struct Msr {
        #define DECMSR(off) \
            uint64_t msr_##off = 0;
        #define DECMSRA(off, val) \
            uint64_t msr_##off = val;
        
        #define RD(off) \
            case off: \
                return msr_##off;
        
        #define WR(off) \
            case off: \
                msr_##off = val; \
                return;

            
        // TODO: maybe make an std::unordered_map
        DECMSR(0x20);
        DECMSR(0x42);
        DECMSR(0xe3);
        DECMSR(0x181);
        DECMSR(0x1e8);
        DECMSR(0x1e9);
        DECMSR(0x450);
        DECMSR(0x4c4);
        DECMSR(0x581);
        DECMSR(0x5dd);
        DECMSR(0x600);
        DECMSR(0x601);
        DECMSR(0x602);
        DECMSR(0x60d);
        DECMSR(0x60e);
        DECMSRA(0x612, 0xf00); // assumed
        DECMSR(0x615);

        uint64_t Read(uint64_t index) {
            switch (index) {
                RD(0x20);
                RD(0x42);
                RD(0xe3);
                RD(0x181);
                RD(0x1e8);
                RD(0x1e9);
                RD(0x450);
                RD(0x4c4);
                RD(0x581);
                RD(0x5dd);
                RD(0x600);
                RD(0x601);
                RD(0x602);
                RD(0x60d);
                RD(0x60e);
                RD(0x612);
                RD(0x615);
                default:
                    fprintf(stderr, "unimplemented read msr 0x%lx\n", index);
                    exit(1);
                    return 0;
            }
        }
        void Write(uint64_t index, uint64_t val) {
            switch (index) {
                WR(0x20);
                WR(0x42);
                WR(0xe3);
                WR(0x181);
                WR(0x1e8);
                WR(0x1e9);
                WR(0x450);
                WR(0x4c4);
                WR(0x581);
                WR(0x5dd);
                WR(0x600);
                WR(0x601);
                WR(0x602);
                WR(0x60d);
                WR(0x60e);
                WR(0x612);
                WR(0x615);
                default:
                    fprintf(stderr, "unimplemented write msr 0x%lx\n", index);
                    exit(1);
                    return;
            }
        }
        void Update(void) {
            // unknown value that the bios waits for
            if (!(msr_0x612 & 0x200)) {
                msr_0x612 |= 0x40;
            }
        }
    } msr; // model-specific reg

    struct Cr {
        uint64_t val;
        uint64_t operator=(uint64_t v) {
            return (val = v);
        }
        enum Type {
            DCR = 0,
            ITM = 1,
            IVA = 2,
            PTA = 8,
            IPSR = 16,
            ISR = 17,
            IIP = 19,
            IFA = 20,
            ITIR = 21,
            IIPA = 22,
            IFS = 23,
            IIM = 24,
            IHA = 25,
            LID = 64,
            IVR = 65,
            TPR = 66,
            EOI = 67,
            IRR0 = 68,
            IRR1 = 69,
            IRR2 = 70,
            IRR3 = 71,
            ITV = 72,
            PMV = 73,
            LRR0 = 80,
            LRR1 = 81,
        };
    } cr[128]; // control register

    uint64_t ip; // instruction pointer

    #pragma pack(push,1)
    struct Cfm {
        struct Rrb {
            uint32_t gr:7; // general reg
            uint32_t fr:7; // float reg
            uint32_t pr:6; // predicate reg
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
            raw = 0;
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
    struct Cpuid {
        uint64_t val;
    } cpuid[5]; // processor identification registers

    static uint64_t MakeVendorInfo(const char *s) {
        return uint64_t(
            ((s[0] & 0xff) << 0x00) | ((s[1] & 0xff) << 0x08) | ((s[2] & 0xff) << 0x10) | ((s[3] & 0xff) << 0x18) |
            (uint64_t(s[4] & 0xff) << 0x20) | (uint64_t(s[5] & 0xff) << 0x28) | 
            (uint64_t(s[6] & 0xff) << 0x30) | (uint64_t(s[7] & 0xff) << 0x38)
            );
    }
    static uint64_t MakeVersionInfo(int num, int rev, int mod, int fam, int archrev) {
        return uint64_t(
            ((num & 0xff) << 0x00) | ((rev & 0xff) << 0x08) | ((mod & 0xff) << 0x10) | ((fam & 0xff) << 0x18) | 
            (uint64_t(archrev & 0xff) << 0x20)
        );
    }

    // TODO
    enum RegType {
        AR_TYPE = 0,
        CPUID_TYPE = 1,
        MSR_TYPE = 2,
        PSR_TYPE = 3,
        CR_TYPE = 4,
        AR_I_TYPE = 5,
    };

    Ia64Regs() {
        gpr[GPR_ZEROREG].val = 0;
        gpr[GPR_ZEROREG].nat = false;
        fpr._val[FPR_ZEROREG].val = Ia64Float(0); // TODO: proper float
        fpr._val[FPR_ONEREG].val = Ia64Float(1); // TODO: proper float
        pr[PR_ONEREG].val = 1;
        for (int i = 1; i < NELEM(gpr); i++) {
            gpr[i].val = uint64_t(0);
            gpr[i].nat = false;
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
        for (int i = 0; i < NELEM(cr); i++) {
            cr[i] = 0;
        }
        psr.raw = 0; // TODO: proper psr
        cpuid[0].val = MakeVendorInfo("Ethanium");
        cpuid[3].val = MakeVersionInfo(4, 0, 0, 0, 0);
        cpuid[1].val = 0;
        cpuid[2].val = 0;
        for (int i = 4; i < (cpuid[3].val & 0xff); i++) {
            cpuid[i].val = 0;
        }
    }

    // If r1 targets an out-of-frame stacked register (as defined by CFM), an illegal operation 
    // fault is delivered, and this function does not return.
    void CheckTargetRegister(uint64_t r) {
        if (r >= (cfm.sof + 32)) {
            //cpu->IllegalOperationFault();
            printf("target register %ld out-of-frame!!\n", r);
            exit(1);
        }
    }
    

    // also called is_read_only_reg in the manual... lol
    bool IsReadOnlyRegister(RegType type, uint64_t idx) {
        switch (type) {
            case AR_TYPE:
                if (idx == Ia64Regs::Ar::Type::BSP) {
                    return true;
                }
                break;
            default:
                debugprintf("TODO: is_read_only_register(%d, %ld)\n", (int)type, idx);
                return false;
        }
        return false;
    }

    bool IsReservedField(RegType type, uint64_t idx, uint64_t val) {
        switch (type) {
            case PSR_TYPE:
                if ((idx == 0) && // PSR_SM, 24 bits
                    (val & 0b000000010001111111000001)) { 
                    return true;
                }
                return false;
            case AR_TYPE:
                if ((idx >= 8) && (idx <= 15)) {
                    return true;
                }
                if (idx == 20) {
                    return true;
                }
                if ((idx == 22) || (idx == 23)) {
                    return true;
                }
                if (idx == 31) {
                    return true;
                }
                if ((idx == 33) || (idx == 34) || (idx == 35)) {
                    return true;
                }
                if ((idx == 37) || (idx == 38) || (idx == 39)) {
                    return true;
                }
                if ((idx == 41) || (idx == 42) || (idx == 43)) {
                    return true;
                }
                if ((idx == 45) || (idx == 46) || (idx == 47)) {
                    return true;
                }
                if ((idx >= 67) && (idx <= 111)) {
                    return true;
                }

                // TODO: actually check for reserved fields
                
                //break;
            default:
                debugprintf("TODO: is_reserved_field(%d, %ld, %ld)\n", (int)type, idx, val);
                return false;
        }
        return false;
    }
    bool IsReservedReg(Ia64Regs::RegType type, uint64_t ar) {
        switch (type) {
            case Ia64Regs::CPUID_TYPE:
                if (ar >= (cpuid[3].val & 0xff)) {
                    return true;
                }
                return false;

            case Ia64Regs::AR_TYPE:
                if ((ar >= 8) && (ar <= 15)) {
                    return true;
                }
                if (ar == 20) {
                    return true;
                }
                if ((ar == 22) || (ar == 23)) {
                    return true;
                }
                if (ar == 31) {
                    return true;
                }
                if ((ar == 33) || (ar == 34) || (ar == 35)) {
                    return true;
                }
                if ((ar == 37) || (ar == 38) || (ar == 39)) {
                    return true;
                }
                if ((ar == 41) || (ar == 42) || (ar == 43)) {
                    return true;
                }
                if ((ar == 45) || (ar == 46) || (ar == 47)) {
                    return true;
                }
                if ((ar >= 67) && (ar <= 111)) {
                    return true;
                }
                break;

            case Ia64Regs::MSR_TYPE:
                return false;
            
            default:
                debugprintf("TODO: is_reserved_reg(%d, %lx)\n", (int)type, ar);
                return false;
        }
        return false;
    }
    uint64_t IgnoredFieldMask(Ia64Regs::RegType type, uint64_t idx, uint64_t val) {
        switch (type) {
            case Ia64Regs::AR_TYPE:
                if ((idx >= 0) && (idx <= 7)) {
                    // kernel registers. no ignored fields
                    return val;
                }
                break;
            case Ia64Regs::MSR_TYPE:
                // probably has no ignored fields
                return val;
        }
        debugprintf("TODO: ignored_field_mask(%d, %ld, %ld)\n", (int)type, idx, val);
        return val;
    }
    // TODO: move those fellas into Ia64Regs::Ar
    bool IsKernelReg(uint64_t idx) {
        return (idx >= 0) && (idx <= 7);
    }
    bool IsIgnoredReg(uint64_t idx) {
        return ((idx >= 48) && (idx <= 63)) || ((idx >= 112) && (idx <= 127));
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
    void RestoreFrame(int a, int b, int c) {
        debugprintf("TODO: rse_restore_frame(%d, %d, %d)\n", a, b, c);
    }
    void EnableCurrentFrameLoad(void) {
        debugprintf("TODO: rse_enable_current_frame_load()\n");
    }
    uint64_t UpdateInternalStackPointers(uint64_t val) {
        debugprintf("TODO: rse_update_internal_stack_pointers(%ld)\n", val);
        return val;
    }
};

enum BiosOffsets {
    PALE_RESET = Memory::BIOS_ROM + 0x3fffb0,
};

class Ia64Cpu {
public:
    Ia64Cpu() {
        regs.ip = PALE_RESET;
        halt = false;
        branched = false;
    } 

    ~Ia64Cpu() {}

    void run(void);
    void Dump(void);

    Ia64Regs regs;
    Ia64Alat alat;
    Ia64Rse rse;
    bool halt;
    bool branched;

    // stubs for now //
    void CheckInterruptRequest(void) {
        printf("TODO: check_interrupt_request()\n");
    }
    void EndOfInterrupt(void) {
        printf("TODO: end_of_interrupt()\n");
    }

    // checks //

    bool UnimplementedVirtualAddress(uint64_t vaddr) {
        // assume all and any virtual address is implemented (for now)
        return false;
    }
    bool UnimplementedPhysicalAddress(uint64_t paddr) {
        // assume all and any physical address is implemented (for now)
        return false;
    }
    // TODO: 
    // traps and faults are interrupts so they branch
    // into the vector 
    // faults //
    void FaultPrint(const char *fault, ...) {
        va_list list;
        va_start(list, fault);
        {
            std::string faultstr = "=================\n\n\nGOT A FAULT: ";
            faultstr += fault;
            faultstr += "\n\n\n=================";
            vfprintf(stderr, faultstr.c_str(), list);
        }
        va_end(list);
    }

    void IllegalOperationFault(void) {
        FaultPrint("Illegal Operation");
        halt = true;
    }

    void ReservedRegisterFieldFault(void) {
        FaultPrint("Reserved Register Field");
        halt = true;
    }

    void RegisterNatConsumptionFault(uint32_t a) {
        FaultPrint("Register NaT Consumption with %d", a);
        halt = true;
    }
    void PrivilegedRegisterFault(void) {
        FaultPrint("Privileged Register");
        halt = true;
    }
    void PrivilegedOperationFault(uint32_t a) {
        FaultPrint("Privileged Operation with %d", a);
        halt = true;
    }


    // traps //
    void TrapPrint(const char *trap, ...) {
        va_list list;
        va_start(list, trap);
        {
            std::string trapstr = "=================\n\n\nGOT A TRAP: ";
            trapstr += trap;
            trapstr += "\n\n\n=================";
            vfprintf(stderr, trapstr.c_str(), list);
        }
        va_end(list);
    }
    
    void UnimplementedInstructionAddressTrap(int a, uint64_t ip) {
        TrapPrint("Unimplmented Insruction Address at %lx", ip);
        halt = true;
    }

    void TakenBranchTrap(void) {
        TrapPrint("Taken Branch");
        halt = true;
    }

    ////////////
};
