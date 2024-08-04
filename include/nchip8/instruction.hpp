// Copyright (c) 2024 inunix3.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace nchip8 {
    enum class InstrKind {
        // CHIP-8 instructions (they are also supported by SCHIP and XO-CHIP)
        CLEAR_SCREEN,
        RET,
        JUMP,
        CALL,
        SKIP_EQUAL,
        SKIP_NOT_EQUAL,
        SKIP_REGS_EQUAL,
        LOAD_BYTE,
        ADD,
        LOAD_REG,
        OR,
        AND,
        XOR,
        ADD_REG,
        SUB_REG,
        RSHIFT,
        LOAD_AND_SUB_REG,
        LSHIFT,
        SKIP_REGS_NOT_EQUAL,
        LOAD_I,
        JUMP_OFFSET,
        RANDOM,
        DRAW_SPRITE,
        SKIP_PRESSED,
        SKIP_NOT_PRESSED,
        LOAD_DT,
        READ_KEY,
        SET_DT,
        SET_ST,
        ADD_I,
        FONT_CHAR,
        BCD,
        REG_DUMP,
        REG_LOAD,

        // SCHIP instructions (XO-CHIP also supports)
        HIRES,
        LORES,
        SCROLL_DOWN,
        SCROLL_RIGHT,
        SCROLL_LEFT,
        BIG_FONT_CHAR,
        SAVE_FLAGS,
        LOAD_FLAGS,
        EXIT
    };

    std::string instrKindToString(InstrKind kind);

    struct OperandMap {
        OperandMap(std::uint16_t opcode);

        // Symbols:
        //   X - first register
        //   Y - second register
        //   N - nibble (immediate value)

        // 0xX000
        std::uint8_t x;

        // 0x0Y00
        std::uint8_t y;

        // 0x0NNN;
        std::uint16_t addr;

        // 0x000N;
        std::uint8_t imm1; // the last 4 bits in the opcode

        // 0x00NN;
        std::uint8_t imm2; // 8-bit immediate value

        // 0x0NNN;
        std::uint16_t imm3; // 12-bit immediate value
    };

    class VM;
    class Instruction {
    public:
        using Impl = std::function<void(VM &, std::uint16_t)>;

        Instruction(InstrKind kind, Impl impl);

        InstrKind kind() const;
        Impl &impl();
        const Impl &impl() const;

    private:
        InstrKind m_kind;
        std::string m_mnemonic;
        Impl m_impl;
    };
}
