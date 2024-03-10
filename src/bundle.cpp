#include <stdio.h>
#include "bundle.hpp"
#include "cpu.hpp"

#include "unit/munit.hpp"
#include "unit/iunit.hpp"
#include "unit/bunit.hpp"
#include "unit/lxunit.hpp"


void Ia64Bundle::Handle(Ia64Cpu *cpu) {
    switch(_template) {
        case Ia64BundleTemplate::MII_0: 
        case Ia64BundleTemplate::MII_1: {
            debugprintf("bundle MII %x\n", _template);
            MUnit::Handle(this, cpu, _instslot0);
            IUnit::Handle(this, cpu, _instslot1);
            IUnit::Handle(this, cpu, _instslot2);
            return;
        }
        case Ia64BundleTemplate::MLX_4: {
            debugprintf("bundle MLX %x\n", _template);
            MUnit::Handle(this, cpu, _instslot0);
            LXUnit::Handle(this, cpu, _instslot1, _instslot2);
            return;
        }
        case Ia64BundleTemplate::MMI_8:
        case Ia64BundleTemplate::MMI_9: {
            debugprintf("bundle MMI %x\n", _template);
            MUnit::Handle(this, cpu, _instslot0);
            MUnit::Handle(this, cpu, _instslot1);
            IUnit::Handle(this, cpu, _instslot2);
            return;
        }
        case Ia64BundleTemplate::MMI_A: {
            debugprintf("bundle MMI %x\n", _template);
            MUnit::Handle(this, cpu, _instslot0);
            MUnit::Handle(this, cpu, _instslot1);
            IUnit::Handle(this, cpu, _instslot2);
            return;
        }
        case Ia64BundleTemplate::MIB_11: {
            debugprintf("bundle MIB %x\n", _template);
            MUnit::Handle(this, cpu, _instslot0);
            IUnit::Handle(this, cpu, _instslot1);
            BUnit::Handle(this, cpu, _instslot2);
            return;
        }
        case Ia64BundleTemplate::MBB_12: {
            debugprintf("bundle MBB %x\n", _template);
            MUnit::Handle(this, cpu, _instslot0);
            BUnit::Handle(this, cpu, _instslot1);
            BUnit::Handle(this, cpu, _instslot2);
            return;
        }
        case Ia64BundleTemplate::MBB_13: {
            debugprintf("bundle MBB %x\n", _template);
            MUnit::Handle(this, cpu, _instslot0);
            BUnit::Handle(this, cpu, _instslot1);
            BUnit::Handle(this, cpu, _instslot2);
            return;
        }
        case Ia64BundleTemplate::MMB_19: {
            debugprintf("bundle MMB %x\n", _template);
            MUnit::Handle(this, cpu, _instslot0);
            MUnit::Handle(this, cpu, _instslot1);
            BUnit::Handle(this, cpu, _instslot2);
            return;
        }
        default: {
            fprintf(stderr, "unimplemented bundle %x!!\n", _template);
            cpu->halt = true;
            return;
        }
    }
}