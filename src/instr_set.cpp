// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/instr_set.hpp>

#include <cstdlib>
#include <limits>

using namespace nchip8;

namespace {
    bool willUnderflowSub(std::uint8_t addend1, std::uint8_t addend2) {
        return addend2 > 0 && addend1 < std::numeric_limits<std::uint8_t>::min() + addend2;
    };
}

void instr_set_impls::clearScreen_impl(VM &vm, std::uint16_t opcode) {
    (void) opcode;

    vm.display.clear();
}

void instr_set_impls::ret_impl(VM &vm, std::uint16_t opcode) {
    (void) opcode;

    auto &stack = vm.state.stack;

    std::uint16_t addr = stack.top();
    stack.pop();
    vm.state.pc = addr;
}

void instr_set_impls::jump_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    vm.state.pc = ops.addr;
}

void instr_set_impls::call_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    auto &stack = vm.state.stack;

    if (stack.size() >= STACK_MAX_SIZE) {
        throw StackOverflow();
    }

    stack.push(vm.state.pc);
    vm.state.pc = ops.addr;
}

void instr_set_impls::skipEqual_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    if (vm.state.regs[ops.x] == ops.imm2) {
        vm.state.pc += 2;
    }
}

void instr_set_impls::skipNotEqual_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    if (vm.state.regs[ops.x] != ops.imm2) {
        vm.state.pc += 2;
    }
}

void instr_set_impls::skipRegsEqual_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    if (vm.state.regs[ops.x] == vm.state.regs[ops.y]) {
        vm.state.pc += 2;
    }
}

void instr_set_impls::loadByte_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    vm.state.regs[ops.x] = ops.imm2;
}

void instr_set_impls::add_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    vm.state.regs[ops.x] += ops.imm2;
}

void instr_set_impls::loadReg_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    vm.state.regs[ops.x] = vm.state.regs[ops.y];
}

void instr_set_impls::or_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    if (vm.quirks.bitwiseResetVF) {
        vm.state.regs[0xf] = 0;
    }

    vm.state.regs[ops.x] |= vm.state.regs[ops.y];
}

void instr_set_impls::and_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    if (vm.quirks.bitwiseResetVF) {
        vm.state.regs[0xf] = 0;
    }

    vm.state.regs[ops.x] &= vm.state.regs[ops.y];
}

void instr_set_impls::xor_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    if (vm.quirks.bitwiseResetVF) {
        vm.state.regs[0xf] = 0;
    }

    vm.state.regs[ops.x] ^= vm.state.regs[ops.y];
}

void instr_set_impls::addReg_impl(VM &vm, std::uint16_t opcode) {
    auto willOverflow = [](std::uint8_t addend1, std::uint8_t addend2) -> bool {
        // See https://stackoverflow.com/a/1514309/12537826
        return addend2 > 0 && addend1 > std::numeric_limits<std::uint8_t>::max() - addend2;
    };

    OperandMap ops(opcode);

    std::uint8_t &vx = vm.state.regs[ops.x];
    std::uint8_t  vy = vm.state.regs[ops.y];

    bool overflow = willOverflow(vx, vy);
    vx += vy;

    // VF is set to 1 when there is a carry (overflow), 0 when there is not.
    vm.state.regs[0xf] = overflow;
}

void instr_set_impls::subReg_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    std::uint8_t &vx = vm.state.regs[ops.x];
    std::uint8_t  vy = vm.state.regs[ops.y];

    bool underflow = willUnderflowSub(vx, vy);
    vx -= vy;

    // VF is set to 0 when there is a borrow (underflow), 1 when there is not.
    vm.state.regs[0xf] = !underflow;
}

void instr_set_impls::rshift_impl(VM &vm, std::uint16_t opcode) {
    auto leastSignificantBit = [](std::uint8_t num) -> std::uint8_t {
        return num & 0b1;
    };

    OperandMap ops(opcode);

    std::uint8_t &vx = vm.state.regs[ops.x];

    if (vm.quirks.shiftSetVxToVy) {
        std::uint8_t vy = vm.state.regs[ops.y];
        vx = vy;
    }

    std::uint8_t lsb = leastSignificantBit(vx);

    vx >>= 1;
    vm.state.regs[0xf] = lsb;
}

void instr_set_impls::loadAndSubReg_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    std::uint8_t &vx = vm.state.regs[ops.x];
    std::uint8_t  vy = vm.state.regs[ops.y];

    bool underflow = willUnderflowSub(vy, vx);
    vx = vy - vx;

    // VF is set to 0 when there is a borrow (underflow), 1 when there is not.
    vm.state.regs[0xf] = !underflow;
}

void instr_set_impls::lshift_impl(VM &vm, std::uint16_t opcode) {
    auto mostSignificantBit = [](std::uint8_t num) -> std::uint8_t {
        return (num >> (sizeof(num) * 8 - 1)) & 0b1;
    };

    OperandMap ops(opcode);

    std::uint8_t &vx = vm.state.regs[ops.x];

    if (vm.quirks.shiftSetVxToVy) {
        std::uint8_t vy = vm.state.regs[ops.y];
        vx = vy;
    }

    std::uint8_t msb = mostSignificantBit(vx);

    vx <<= 1;
    vm.state.regs[0xf] = msb;
}

void instr_set_impls::skipRegsNotEqual_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    if (vm.state.regs[ops.x] != vm.state.regs[ops.y]) {
        vm.state.pc += 2;
    }
}

void instr_set_impls::loadI_impl(VM &vm, std::uint16_t opcode) {
    std::uint16_t addr = opcode & 0x0fff;

    vm.state.i = addr;
}

void instr_set_impls::jumpOffset_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    std::uint8_t offset;

    if (vm.quirks.jumpOffsetUseV0) {
        offset = vm.state.regs[0];
    } else {
        offset = vm.state.regs[ops.x];
    }

    vm.state.i = ops.addr + offset;
}

void instr_set_impls::random_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    vm.state.regs[ops.x] = std::rand() & ops.imm2;
}

void instr_set_impls::drawSprite_impl(VM &vm, std::uint16_t opcode) {
    auto drawPixel = [&vm = vm](sdl::Point pos, int spriteX) {
        pos.x += spriteX;

        if (pos.x >= DISPLAY_SIZE.x) {
            if (vm.quirks.wrapPixels) {
                pos.x %= DISPLAY_SIZE.y;
            } else {
                return;
            }
        }

        PixelState pxState = vm.display.at(pos);

        if (pxState == PixelState::ON) {
            vm.state.regs[0xf] = 1;
        }

        vm.display.setPixel(pos, !pxState);
    };

    OperandMap ops(opcode);
    std::uint8_t x = vm.state.regs[ops.x] % DISPLAY_SIZE.x;
    std::uint8_t y = vm.state.regs[ops.y] % DISPLAY_SIZE.y;
    std::uint8_t height = ops.imm1;

    vm.state.regs[0xf] = 0;

    for (uint8_t i = 0; i < height; ++i) {
        std::uint8_t spriteRow = vm.state.memory[vm.state.i + i];

        for (std::uint8_t j = 0; j < 8; ++j) {
            if ((spriteRow & (0x80 >> j))) {
                drawPixel({ j, y + i }, x);
            }
        }
    }
}

void instr_set_impls::skipPressed_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    const auto &inputTable = vm.state.inputTable;
    auto key = vm.state.regs[ops.x];

    if (inputTable[key]) {
        vm.state.pc += 2;
    }
}

void instr_set_impls::skipNotPressed_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    const auto &inputTable = vm.state.inputTable;
    auto key = vm.state.regs[ops.x];

    if (!inputTable[key]) {
        vm.state.pc += 2;
    }
}

void instr_set_impls::loadDT_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    vm.state.regs[ops.x] = vm.state.dt;
}

void instr_set_impls::readKey_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    const auto &table = vm.state.inputTable;

    for (std::size_t i = 0; i < table.size(); ++i) {
        if (table[i]) {
            vm.waitForKeyRelease = true;

            break;
        }

        if (vm.waitForKeyRelease && !table[i]) {
            vm.state.regs[ops.x] = (std::uint8_t) i;
            vm.waitForKeyRelease = false;

            return;
        }
    }

    vm.state.pc -= 2;
}

void instr_set_impls::setDT_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    vm.state.dt = vm.state.regs[ops.x];
}

void instr_set_impls::setST_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    vm.state.st = vm.state.regs[ops.x];
}

void instr_set_impls::addI_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    vm.state.i += vm.state.regs[ops.x];
}

void instr_set_impls::fontChar_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    std::uint8_t vx = vm.state.regs[ops.x];
    vm.state.i = FONT_OFFSET + vx * FONT_CHAR_SIZE.y;
}

void instr_set_impls::bcd_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    auto bcd = [](std::uint16_t num, int digit) -> std::uint8_t {
        for (int i = 1; i < digit; ++i) {
            num /= 10;
        }

        return num % 10;
    };

    auto &memory = vm.state.memory;
    auto i = vm.state.i;
    auto vx = vm.state.regs[ops.x];

    memory[i + 0] = bcd(vx, 3); // hundreds digit
    memory[i + 1] = bcd(vx, 2); // tens digit
    memory[i + 2] = bcd(vx, 1); // ones digit
}

void instr_set_impls::regDump_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    const auto &regs = vm.state.regs;
    auto &regI = vm.state.i;

    for (std::size_t i = 0; i <= ops.x; ++i) {
        vm.state.memory[regI + i] = regs[i];
    }

    if (vm.quirks.loadSaveIncrementI) {
        regI += ops.x + 1;
    }
}

void instr_set_impls::regLoad_impl(VM &vm, std::uint16_t opcode) {
    OperandMap ops(opcode);

    auto &regs = vm.state.regs;
    auto &regI = vm.state.i;

    for (std::size_t i = 0; i <= ops.x; ++i) {
        regs[i] = vm.state.memory[regI + i];
    }

    if (vm.quirks.loadSaveIncrementI) {
        regI += ops.x + 1;
    }
}