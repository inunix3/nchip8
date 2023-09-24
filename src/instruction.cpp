// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/instruction.hpp>

using namespace nchip8;

std::string nchip8::instrKindToString(InstrKind kind) {
    switch (kind) {
    case InstrKind::CLEAR_SCREEN:
        return "CLEAR_SCREEN";
    case InstrKind::RET:
        return "RET";
    case InstrKind::JUMP:
        return "JUMP";
    case InstrKind::CALL:
        return "CALL";
    case InstrKind::SKIP_EQUAL:
        return "SKIP_EQUAL";
    case InstrKind::SKIP_NOT_EQUAL:
        return "SKIP_NOT_EQUAL";
    case InstrKind::SKIP_REGS_EQUAL:
        return "SKIP_REGS_EQUAL";
    case InstrKind::LOAD_BYTE:
        return "LOAD_BYTE";
    case InstrKind::ADD:
        return "ADD";
    case InstrKind::LOAD_REG:
        return "LOAD_REG";
    case InstrKind::OR:
        return "OR";
    case InstrKind::AND:
        return "AND";
    case InstrKind::XOR:
        return "XOR";
    case InstrKind::ADD_REG:
        return "ADD_REG";
    case InstrKind::SUB_REG:
        return "SUB_REG";
    case InstrKind::RSHIFT:
        return "RSHIFT";
    case InstrKind::LOAD_AND_SUB_REG:
        return "LOAD_AND_SUB_REG";
    case InstrKind::LSHIFT:
        return "LSHIFT";
    case InstrKind::SKIP_REGS_NOT_EQUAL:
        return "SKIP_REGS_NOT_EQUAL";
    case InstrKind::LOAD_I:
        return "LOAD_I";
    case InstrKind::JUMP_OFFSET:
        return "JUMP_OFFSET";
    case InstrKind::RANDOM:
        return "RANDOM";
    case InstrKind::DRAW_SPRITE:
        return "DRAW_SPRITE";
    case InstrKind::SKIP_PRESSED:
        return "SKIP_PRESSED";
    case InstrKind::SKIP_NOT_PRESSED:
        return "SKIP_NOT_PRESSED";
    case InstrKind::LOAD_DT:
        return "LOAD_DT";
    case InstrKind::READ_KEY:
        return "READ_KEY";
    case InstrKind::SET_DT:
        return "SET_DT";
    case InstrKind::SET_ST:
        return "SET_ST";
    case InstrKind::ADD_I:
        return "ADD_I";
    case InstrKind::FONT_CHAR:
        return "FONT_CHAR";
    case InstrKind::BCD:
        return "BCD";
    case InstrKind::REG_DUMP:
        return "REG_DUMP";
    case InstrKind::REG_LOAD:
        return "REG_LOAD";
    case InstrKind::HIRES:
        return "HIRES";
    case InstrKind::LORES:
        return "LORES";
    case InstrKind::SCROLL_DOWN:
        return "SCROLL_DOWN";
    case InstrKind::SCROLL_RIGHT:
        return "SCROLL_RIGHT";
    case InstrKind::SCROLL_LEFT:
        return "SCROLL_LEFT";
    case InstrKind::BIG_FONT_CHAR:
        return "BIG_FONT_CHAR";
    case InstrKind::SAVE_FLAGS:
        return "SAVE_FLAGS";
    case InstrKind::LOAD_FLAGS:
        return "LOAD_FLAGS";
    case InstrKind::EXIT:
        return "EXIT";
    }
}

OperandMap::OperandMap(std::uint16_t opcode)
    : x    { (uint8_t)  ((opcode & 0x0f00) >> 8) },
      y    { (uint8_t)  ((opcode & 0x00f0) >> 4) },
      addr { (uint16_t)  (opcode & 0x0fff) },
      imm1 { (uint8_t)   (opcode & 0x000f) },
      imm2 { (uint8_t)   (opcode & 0x00ff) },
      imm3 { (uint16_t)  (opcode & 0x0fff) } {
}

Instruction::Instruction(InstrKind kind, Impl impl)
    : m_kind { kind }, m_impl { impl } {
}

InstrKind Instruction::kind() const {
    return m_kind;
}

Instruction::Impl &Instruction::impl() {
    return m_impl;
}

const Instruction::Impl &Instruction::impl() const {
    return m_impl;
}
