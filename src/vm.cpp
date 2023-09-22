// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/vm.hpp>
#include <nchip8/instr_set.hpp>
#include <nchip8/utils.hpp>

#include <cstring>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

using namespace nchip8;

VMError::VMError(const std::string &err)
    : std::runtime_error { err } {

}

InvalidOpcode::InvalidOpcode(std::uint16_t opcode, std::uint16_t offset)
    : VMError { "invalid opcode " + utils::toHexPrefixed(opcode) + " at " + utils::toHexPrefixed(offset) } {
}

StackOverflow::StackOverflow()
    : VMError { "the maximum number of values in the stack (" + std::to_string(STACK_MAX_SIZE) + ") has been exceeded" } {

}

VMState::VMState() :
    romSize { 0 } {

    static const std::uint8_t font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    memory.fill(0);
    std::memcpy(&memory[FONT_OFFSET], font, sizeof(font) / sizeof(font[0]));

    reset();
}

void VMState::updateTimers() {
    if (dt > 0) {
        --dt;
    }

    if (st > 0) {
        --st;
    }
}

void VMState::reset() {
    pc = PROG_OFFSET;
    dt = UINT8_MAX;
    st = 0;
    i  = 0;

    regs.fill(0);

    while (!stack.empty()) {
        stack.pop();
    }

    inputTable.reset();
}

VM::VM(Display &display, const Config &cfg)
    : cfg { cfg },
      display { display },
      beeper { cfg.sound.waveform, cfg.sound.level, cfg.sound.frequency } {
    using namespace instr_set_impls;

    static const Instruction instrs[] = {
        Instruction(InstrKind::CLEAR_SCREEN,        clearScreen_impl),
        Instruction(InstrKind::RET,                 ret_impl),
        Instruction(InstrKind::JUMP,                jump_impl),
        Instruction(InstrKind::CALL,                call_impl),
        Instruction(InstrKind::SKIP_EQUAL,          skipEqual_impl),
        Instruction(InstrKind::SKIP_NOT_EQUAL,      skipNotEqual_impl),
        Instruction(InstrKind::SKIP_REGS_EQUAL,     skipRegsEqual_impl),
        Instruction(InstrKind::LOAD_BYTE,           loadByte_impl),
        Instruction(InstrKind::ADD,                 add_impl),
        Instruction(InstrKind::LOAD_REG,            loadReg_impl),
        Instruction(InstrKind::OR,                  or_impl),
        Instruction(InstrKind::AND,                 and_impl),
        Instruction(InstrKind::XOR,                 xor_impl),
        Instruction(InstrKind::ADD_REG,             addReg_impl),
        Instruction(InstrKind::SUB_REG,             subReg_impl),
        Instruction(InstrKind::RSHIFT,              rshift_impl),
        Instruction(InstrKind::LOAD_AND_SUB_REG,    loadAndSubReg_impl),
        Instruction(InstrKind::LSHIFT,              lshift_impl),
        Instruction(InstrKind::SKIP_REGS_NOT_EQUAL, skipRegsNotEqual_impl),
        Instruction(InstrKind::LOAD_I,              loadI_impl),
        Instruction(InstrKind::JUMP_OFFSET,         jumpOffset_impl),
        Instruction(InstrKind::RANDOM,              random_impl),
        Instruction(InstrKind::DRAW_SPRITE,         drawSprite_impl),
        Instruction(InstrKind::SKIP_PRESSED,        skipPressed_impl),
        Instruction(InstrKind::SKIP_NOT_PRESSED,    skipNotPressed_impl),
        Instruction(InstrKind::LOAD_DT,             loadDT_impl),
        Instruction(InstrKind::READ_KEY,            readKey_impl),
        Instruction(InstrKind::SET_DT,              setDT_impl),
        Instruction(InstrKind::SET_ST,              setST_impl),
        Instruction(InstrKind::ADD_I,               addI_impl),
        Instruction(InstrKind::FONT_CHAR,           fontChar_impl),
        Instruction(InstrKind::BCD,                 bcd_impl),
        Instruction(InstrKind::REG_DUMP,            regDump_impl),
        Instruction(InstrKind::REG_LOAD,            regLoad_impl)
    };

    for (const auto &instr : instrs) {
        m_instrSet.insert({ instr.kind(), instr });
    }
}

void VM::update() {
    if (m_mode == VMMode::EMPTY) {
        return;
    }

    auto updateTimers = [&](std::uint32_t currentTime) {
        static std::uint32_t lastlyTimersUpdated = SDL_GetTicks();
        static int counter = 0;

        std::uint32_t deltaTime = currentTime - lastlyTimersUpdated;
        counter += deltaTime;

        while (counter >= TIMER_UPDATE_FREQ) {
            state.updateTimers();

            counter -= TIMER_UPDATE_FREQ;
            lastlyTimersUpdated = currentTime;
        }
    };

    static std::uint32_t lastlyStepped = SDL_GetTicks();
    std::uint32_t currentTime = SDL_GetTicks();
    std::uint32_t deltaTime   = currentTime - lastlyStepped;

    if (m_mode == VMMode::RUN && (deltaTime >= 1000 / cfg.cpu.cyclesPerSec)) {
        lastlyStepped = currentTime;

        if (breakpoints.has(state.pc)) {
            m_mode = VMMode::STEP;

            return;
        }

        step();
    }

    updateTimers(currentTime);

    if (cfg.sound.enable && m_mode == VMMode::RUN && state.st > 0) {
        beeper.play();
    }
}

void VM::step() {
    if (m_mode == VMMode::EMPTY) {
        return;
    }

    // Fetch opcode
    std::uint16_t opcode = *(const std::uint16_t *const) &state.memory[state.pc];
    opcode = utils::swapEndianess(opcode); // system endianess agnostic

    state.pc += 2;

    try {
        execInstr(opcode);
    } catch (const VMError &err) {
        m_mode = VMMode::PAUSED;

        throw err;
    }
}

void VM::updateInputTable(const SDL_Event &event) {
    auto &inputTable = state.inputTable;
    const auto &keysym = event.key.keysym;

    // Ctrl+X and just X are different things!
    if (keysym.mod != KMOD_NONE) {
        return;
    }

    for (const auto &key : cfg.input.layout) {
        if (keysym.scancode == key.first) {
            inputTable[(std::size_t) key.second] = event.type == SDL_KEYDOWN;

            break;
        }
    }
};

void VM::setExtension(Extension ext) {
    m_ext = ext;

    m_instrSet.clear();
    loadInstrSet(ext);
}

void VM::execInstr(std::uint16_t opcode) {
    InstrKind instr = decodeOpcode(opcode);

    Instruction::Impl impl = m_instrSet.at(instr).impl();
    impl(*this, opcode);
}

std::optional<InstrKind> VM::tryDecodeOpcode(std::uint16_t opcode) {
    if ((opcode & 0xfff0) == 0x00e0) {
        switch (opcode & 0x000f) {
        case 0x0: return InstrKind::CLEAR_SCREEN;
        case 0xe: return InstrKind::RET;
        }
    }

    switch (opcode & 0xf000) {
    case 0x1000: return InstrKind::JUMP;
    case 0x2000: return InstrKind::CALL;
    case 0x3000: return InstrKind::SKIP_EQUAL;
    case 0x4000: return InstrKind::SKIP_NOT_EQUAL;
    case 0x5000: return InstrKind::SKIP_REGS_EQUAL;
    case 0x6000: return InstrKind::LOAD_BYTE;
    case 0x7000: return InstrKind::ADD;
    case 0x9000: return InstrKind::SKIP_REGS_NOT_EQUAL;
    case 0xa000: return InstrKind::LOAD_I;
    case 0xb000: return InstrKind::JUMP_OFFSET;
    case 0xc000: return InstrKind::RANDOM;
    case 0xd000: return InstrKind::DRAW_SPRITE;
    }

    switch (opcode & 0xf00f) {
    case 0x8000: return InstrKind::LOAD_REG;
    case 0x8001: return InstrKind::OR;
    case 0x8002: return InstrKind::AND;
    case 0x8003: return InstrKind::XOR;
    case 0x8004: return InstrKind::ADD_REG;
    case 0x8005: return InstrKind::SUB_REG;
    case 0x8006: return InstrKind::RSHIFT;
    case 0x8007: return InstrKind::LOAD_AND_SUB_REG;
    case 0x800e: return InstrKind::LSHIFT;
    }

    switch (opcode & 0xf0ff) {
    case 0xe09e: return InstrKind::SKIP_PRESSED;
    case 0xe0a1: return InstrKind::SKIP_NOT_PRESSED;
    case 0xf007: return InstrKind::LOAD_DT;
    case 0xf00a: return InstrKind::READ_KEY;
    case 0xf015: return InstrKind::SET_DT;
    case 0xf018: return InstrKind::SET_ST;
    case 0xf01e: return InstrKind::ADD_I;
    case 0xf029: return InstrKind::FONT_CHAR;
    case 0xf033: return InstrKind::BCD;
    case 0xf055: return InstrKind::REG_DUMP;
    case 0xf065: return InstrKind::REG_LOAD;
    }

    return std::nullopt;
}

void VM::load(std::vector<std::uint8_t> rom) {
    if (rom.size() > PROG_MAX_SIZE) {
        throw std::length_error("Size of program must be <= " + std::to_string(PROG_MAX_SIZE) + " bytes");
    }

    std::memcpy(&state.memory[PROG_OFFSET], rom.data(), rom.size());
    state.romSize = rom.size();

    reset();
}

void VM::loadFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        throw std::runtime_error("file '" + filename + "' cannot be opened. May not exist or may not have read permission");
    }

    // Don't skip whitespaces
    file.unsetf(std::ios::skipws);

    // Getting file size using the traditional method
    file.seekg(0, std::ios::end);
    auto fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<std::uint8_t> prog;
    prog.resize((std::size_t) fileSize);

    file.read((char *) prog.data(), fileSize);

    load(prog);
}

void VM::reset() {
    state.reset();
    display.clear();
}

void VM::unload() {
    // Fill memory with zeros except for the font space
    std::memset(&state.memory[FONT_MEM_SIZE], 0, MEM_SIZE - FONT_MEM_SIZE);
    state.romSize = 0;
    reset();
}

std::string VM::disassemble(std::uint16_t opcode) {
    InstrKind kind = decodeOpcode(opcode);
    OperandMap ops(opcode);
    std::stringstream str;

    std::string xReg = "V" + utils::toHexUpper<std::uint8_t, 1>(ops.x);
    std::string yReg = "V" + utils::toHexUpper<std::uint8_t, 1>(ops.y);

    switch (kind) {
    case InstrKind::CLEAR_SCREEN:
        str << "clear_screen";

        break;
    case InstrKind::RET:
        str << "ret";

        break;
    case InstrKind::JUMP:
        str << "jump " << utils::toHexPrefixed(ops.addr);

        break;
    case InstrKind::CALL:
        str << "call " << utils::toHexPrefixed(ops.addr);

        break;
    case InstrKind::SKIP_EQUAL:
        str << "skip_equal " << xReg << ", " << utils::toHexPrefixed(ops.imm2);

        break;
    case InstrKind::SKIP_NOT_EQUAL:
        str << "skip_not_equal " << xReg << ", " << utils::toHexPrefixed(ops.imm2);

        break;
    case InstrKind::SKIP_REGS_EQUAL:
        str << "skip_equal "<< xReg << ", " << yReg;

        break;
    case InstrKind::LOAD_BYTE:
        str << "load " << xReg << ", " << utils::toHexPrefixed(ops.imm2);

        break;
    case InstrKind::ADD:
        str << "add " << xReg << ", " << utils::toHexPrefixed(ops.imm2);

        break;
    case InstrKind::LOAD_REG:
        str << "load " << xReg << ", " << yReg;

        break;
    case InstrKind::OR:
        str << "or " << xReg << ", " << yReg;

        break;
    case InstrKind::AND:
        str << "and " << xReg << ", " << yReg;

        break;
    case InstrKind::XOR:
        str << "xor " << xReg << ", " << yReg;

        break;
    case InstrKind::ADD_REG:
        str << "add " << xReg << ", " << yReg;

        break;
    case InstrKind::SUB_REG:
        str << "sub " << xReg << ", " << yReg;

        break;
    case InstrKind::RSHIFT:
        str << "rshift " << xReg;

        break;
    case InstrKind::LOAD_AND_SUB_REG:
        str << "ldsub " << xReg << ", " << yReg;

        break;
    case InstrKind::LSHIFT:
        str << "lshift " << xReg;

        break;
    case InstrKind::SKIP_REGS_NOT_EQUAL:
        str << "skip_not_equal " << xReg << ", " << yReg;

        break;
    case InstrKind::LOAD_I:
        str << "load I, " << utils::toHexPrefixed(ops.imm3);

        break;
    case InstrKind::JUMP_OFFSET:
        str << "jump " << utils::toHexPrefixed(ops.addr) << " + ";

        if (quirks.jumpOffsetUseV0) {
            str << "V0";
        } else {
            str << xReg;
        }

        break;
    case InstrKind::RANDOM:
        str << "random " << utils::toHexPrefixed(ops.imm2);

        break;
    case InstrKind::DRAW_SPRITE:
        str << "draw_sprite " << xReg << ", " << yReg << ", " << utils::toHexPrefixed(ops.imm1);

        break;
    case InstrKind::SKIP_PRESSED:
        str << "skip_pressed " << xReg;

        break;
    case InstrKind::SKIP_NOT_PRESSED:
        str << "skip_not_pressed " << xReg;

        break;
    case InstrKind::LOAD_DT:
        str << "load_dt " << xReg;

        break;
    case InstrKind::READ_KEY:
        str << "wait_keypress " << xReg;

        break;
    case InstrKind::SET_DT:
        str << "load DT, " << xReg;

        break;
    case InstrKind::SET_ST:
        str << "load ST, " << xReg;

        break;
    case InstrKind::ADD_I:
        str << "add I, " << xReg;

        break;
    case InstrKind::FONT_CHAR:
        str << "load I, font[" << xReg << ']';

        break;
    case InstrKind::BCD:
        str << "bcd " << xReg;

        break;
    case InstrKind::REG_DUMP:
        str << "reg_dump " << xReg;

        break;
    case InstrKind::REG_LOAD:
        str << "reg_load " << xReg;

        break;
    }

    return str.str();
}

void VM::setMode(VMMode mode) {
    if (m_mode == mode) {
        return;
    }

    m_prevMode = m_mode;
    m_mode = mode;
}

VMMode VM::prevMode() const {
    return m_prevMode;
}

VMMode VM::mode() const {
    return m_mode;
}

Extension VM::ext() const {
    return m_ext;
}

InstrKind VM::decodeOpcode(std::uint16_t opcode) {
    auto kind = tryDecodeOpcode(opcode);

    if (!kind) {
        std::uint16_t offset = state.pc > 0 ? state.pc - 2 : 0;

        throw InvalidOpcode(opcode, offset);
    }

    return kind.value();
}

void VM::loadInstrSet(Extension ext) {
    using namespace instr_set_impls;

    static const Instruction instrs[] = {
        Instruction(InstrKind::CLEAR_SCREEN,        clearScreen_impl),
        Instruction(InstrKind::RET,                 ret_impl),
        Instruction(InstrKind::JUMP,                jump_impl),
        Instruction(InstrKind::CALL,                call_impl),
        Instruction(InstrKind::SKIP_EQUAL,          skipEqual_impl),
        Instruction(InstrKind::SKIP_NOT_EQUAL,      skipNotEqual_impl),
        Instruction(InstrKind::SKIP_REGS_EQUAL,     skipRegsEqual_impl),
        Instruction(InstrKind::LOAD_BYTE,           loadByte_impl),
        Instruction(InstrKind::ADD,                 add_impl),
        Instruction(InstrKind::LOAD_REG,            loadReg_impl),
        Instruction(InstrKind::OR,                  or_impl),
        Instruction(InstrKind::AND,                 and_impl),
        Instruction(InstrKind::XOR,                 xor_impl),
        Instruction(InstrKind::ADD_REG,             addReg_impl),
        Instruction(InstrKind::SUB_REG,             subReg_impl),
        Instruction(InstrKind::RSHIFT,              rshift_impl),
        Instruction(InstrKind::LOAD_AND_SUB_REG,    loadAndSubReg_impl),
        Instruction(InstrKind::LSHIFT,              lshift_impl),
        Instruction(InstrKind::SKIP_REGS_NOT_EQUAL, skipRegsNotEqual_impl),
        Instruction(InstrKind::LOAD_I,              loadI_impl),
        Instruction(InstrKind::JUMP_OFFSET,         jumpOffset_impl),
        Instruction(InstrKind::RANDOM,              random_impl),
        Instruction(InstrKind::DRAW_SPRITE,         drawSprite_impl),
        Instruction(InstrKind::SKIP_PRESSED,        skipPressed_impl),
        Instruction(InstrKind::SKIP_NOT_PRESSED,    skipNotPressed_impl),
        Instruction(InstrKind::LOAD_DT,             loadDT_impl),
        Instruction(InstrKind::READ_KEY,            readKey_impl),
        Instruction(InstrKind::SET_DT,              setDT_impl),
        Instruction(InstrKind::SET_ST,              setST_impl),
        Instruction(InstrKind::ADD_I,               addI_impl),
        Instruction(InstrKind::FONT_CHAR,           fontChar_impl),
        Instruction(InstrKind::BCD,                 bcd_impl),
        Instruction(InstrKind::REG_DUMP,            regDump_impl),
        Instruction(InstrKind::REG_LOAD,            regLoad_impl)
    };
    for (const auto &instr : instrs) {
        m_instrSet.insert({ instr.kind(), instr });
    }
