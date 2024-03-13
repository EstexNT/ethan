#include <array>
#include "cpu.hpp"

#include "unit/munit.hpp"
#include "unit/iunit.hpp"
#include "unit/bunit.hpp"
#include "unit/lxunit.hpp"


std::array<HandleFn, 3> Ia64Bundle::Handle(Ia64Cpu *cpu) {
    std::array<HandleFn, 3> out {};

    Ia64Format format0 {this, 0}; 
    Ia64Format format1 {this, 1}; 
    Ia64Format format2 {this, 2};

    switch(_template) {
        case Ia64BundleTemplate::MII_0: 
        case Ia64BundleTemplate::MII_1: {
            debugprintf("bundle MII %x\n", _template);
            out[0] = MUnit::Handle(&format0, cpu);
            out[1] = IUnit::Handle(&format1, cpu);
            out[2] = IUnit::Handle(&format2, cpu);
            break;
        }
        case Ia64BundleTemplate::MLX_4: {
            debugprintf("bundle MLX %x\n", _template);
            out[0] = MUnit::Handle(&format0, cpu);
            auto lx = LXUnit::Handle(&format1, &format2, cpu);
            out[1] = lx.first;
            out[2] = lx.second;
            break;
        }
        case Ia64BundleTemplate::MMI_8: 
        case Ia64BundleTemplate::MMI_9: 
        case Ia64BundleTemplate::MMI_A: {
            debugprintf("bundle MMI %x\n", _template);
            out[0] = MUnit::Handle(&format0, cpu);
            out[1] = MUnit::Handle(&format1, cpu);
            out[2] = IUnit::Handle(&format2, cpu);
            break;
        }
        case Ia64BundleTemplate::MIB_11: {
            debugprintf("bundle MIB %x\n", _template);
            out[0] = MUnit::Handle(&format0, cpu);
            out[1] = IUnit::Handle(&format1, cpu);
            out[2] = BUnit::Handle(&format2, cpu);
            break;
        }
        case Ia64BundleTemplate::MBB_12: 
        case Ia64BundleTemplate::MBB_13: {
            debugprintf("bundle MBB %x\n", _template);
            out[0] = MUnit::Handle(&format0, cpu);
            out[1] = BUnit::Handle(&format1, cpu);
            out[2] = BUnit::Handle(&format2, cpu);
            break;
        }
        case Ia64BundleTemplate::MMB_19: {
            debugprintf("bundle MMB %x\n", _template);
            out[0] = MUnit::Handle(&format0, cpu);
            out[1] = MUnit::Handle(&format1, cpu);
            out[2] = BUnit::Handle(&format2, cpu);
            break;
        }
        default: {
            fprintf(stderr, "unimplemented bundle %x!!\n", _template);
            exit(1);
        }
    }
    return out;
}
