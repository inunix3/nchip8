// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#pragma once


#include "breakpoint.hpp"
#include "config.hpp"
#include "display.hpp"
#include "instruction.hpp"
#include "sdl.hpp"
#include "waveform_generator.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <stack>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace nchip8 {
    class VMError : public std::runtime_error {
    public:
        VMError(const std::string &err);
    };

    class InvalidOpcode : public VMError {
    public:
        InvalidOpcode(uint16_t opcode, uint16_t offset);
    };

    class StackOverflow : public VMError {
    public:
        StackOverflow();
    };

    template<typename T, typename Container = std::vector<T>>
    class VMStack : public std::stack<T, Container> {
    public:
        // Expose the member c, because we'll need it to print the data
        // (see Stack View in src/ui.cpp).
        using std::stack<T, Container>::c;
    };

    inline constexpr std::size_t   MEM_SIZE       = 4096;
    inline constexpr std::uint16_t PROG_OFFSET    = 0x0200;
    inline constexpr std::size_t   PROG_MAX_SIZE  = MEM_SIZE - PROG_OFFSET;
    inline constexpr std::uint16_t FONT_OFFSET    = 0x0;
    inline constexpr sdl::Point    FONT_CHAR_SIZE = { 4, 5 };
    inline constexpr std::size_t   FONT_MEM_SIZE  = FONT_CHAR_SIZE.y * 16;
    inline constexpr std::size_t   STACK_MAX_SIZE = 12;
    inline constexpr std::uint16_t BIG_FONT_OFFSET    = FONT_MEM_SIZE;
    inline constexpr sdl::Point    BIG_FONT_CHAR_SIZE = { 8, 10 };
    inline constexpr std::size_t   BIG_FONT_MEM_SIZE  = BIG_FONT_CHAR_SIZE.y * 16;
    inline constexpr int TIMER_UPDATE_FREQ = 1000 / 60;

    struct VMState {
        VMState();

        void updateTimers();
        void reset();

        std::uint16_t pc; // program (instruction) counter
        std::uint8_t  dt; // delay timer
        std::uint8_t  st; // sound timer
        std::uint16_t  i; // address register
        std::array<std::uint8_t, 16> regs;
        VMStack<std::uint16_t> stack;

        std::array<std::uint8_t, MEM_SIZE> memory;
        std::size_t romSize;

        std::bitset<KEY_COUNT> inputTable;
    };

    enum class VMMode {
        EMPTY,
        RUN,
        STEP,
        PAUSED
    };

    struct Quirks {
        bool jumpOffsetUseV0    = true;
        bool wrapPixels         = false;
        bool bitwiseResetVF     = false;
        bool shiftSetVxToVy     = true;
        bool loadSaveIncrementI = true;
    enum class Extension {
        NONE,
        SCHIP
    };

    class VM {
    public:
        VM(Display &display, const Config &cfg);

        // Steps and updates the timers
        void update();
        void step();
        void updateInputTable(const SDL_Event &event);
        void setExtension(Extension ext);

        void execInstr(std::uint16_t opcode);
        std::optional<InstrKind> tryDecodeOpcode(std::uint16_t);

        void load(std::vector<std::uint8_t> rom);
        void loadFile(const std::string &filename);

        void reset();
        void unload();

        std::string disassemble(std::uint16_t opcode);

        void setMode(VMMode mode);
        VMMode prevMode() const;
        VMMode mode() const;
        Extension ext() const;

        VMState state;
        Config cfg;
        Quirks quirks;
        Display &display;
        WaveformGenerator beeper;

        BreakpointMap breakpoints;
        bool waitForKeyRelease = false;
        std::size_t keyToRelease = 0;

    private:
        InstrKind decodeOpcode(std::uint16_t opcode);
        void loadInstrSet(Extension ext);

        std::unordered_map<InstrKind, Instruction> m_instrSet;
        VMMode m_mode = VMMode::EMPTY;
        VMMode m_prevMode;
        Extension m_ext = Extension::NONE;
    };
}
