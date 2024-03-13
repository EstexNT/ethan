#include "lxunit.hpp"

namespace LXUnit {

DECLINST(UnimplInst) {
    fprintf(stderr, "unimpl l+x op=%d\n", format->common.op);
    cpu->halt = true;
}

HandleFn handletable[16] = {
    UnimplInst, UnimplInst, UnimplInst, UnimplInst, 
    UnimplInst, UnimplInst, UnimplInst, UnimplInst, 
    UnimplInst, UnimplInst, UnimplInst, UnimplInst, 
    UnimplInst, UnimplInst, UnimplInst, UnimplInst, 
};


} // LXUnit
