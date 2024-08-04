// Copyright (c) 2024 inunix3.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#include "vm.hpp"

namespace nchip8::instr_set_impls {
#define DECL(_name) void _name##_impl(VM &vm, std::uint16_t opcode)
    // CHIP-8 instructions
    DECL(clearScreen);
    DECL(ret);
    DECL(jump);
    DECL(call);
    DECL(skipEqual);
    DECL(skipNotEqual);
    DECL(skipRegsEqual);
    DECL(loadByte);
    DECL(add);
    DECL(loadReg);
    DECL(or);
    DECL(and);
    DECL(xor);
    DECL(addReg);
    DECL(subReg);
    DECL(rshift);
    DECL(loadAndSubReg);
    DECL(lshift);
    DECL(skipRegsNotEqual);
    DECL(loadI);
    DECL(jumpOffset);
    DECL(random);
    DECL(drawSprite);
    DECL(skipPressed);
    DECL(skipNotPressed);
    DECL(loadDT);
    DECL(readKey);
    DECL(setDT);
    DECL(setST);
    DECL(addI);
    DECL(fontChar);
    DECL(bcd);
    DECL(regDump);
    DECL(regLoad);

    // SCHIP instructions
    DECL(hires);
    DECL(lores);
    DECL(scrollDown);
    DECL(scrollRight);
    DECL(scrollLeft);
    DECL(bigFontChar);
    DECL(saveFlags);
    DECL(loadFlags);
    DECL(exit);
#undef DECL
}
