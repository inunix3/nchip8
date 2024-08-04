// Copyright (c) 2024 inunix3.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#include "breakpoints.hpp"
#include "disassembler.hpp"
#include "instr_executor.hpp"
#include "keypad.hpp"
#include "registers.hpp"
#include "settings.hpp"
#include "stack.hpp"
#include "ui_style.hpp"
#include "../imgui.hpp"
#include "../sdl.hpp"
#include "../vm.hpp"

#include <string>

namespace nchip8::ui {
    class UI {
    public:
        UI(sdl::Window &window, sdl::Renderer &renderer, VM &vm);
        ~UI();

        void update();
        void showError(const std::string &err);
        void render();
        void setStyle(UIStyle style);
        bool wantCaptureKeyboard() const;
        bool quitRequested() const;

    private:
        void input();

        void pauseScreen();
        void popups();
        void menu();

        void mainMenu();
        void about();

        void windows();

        ImGuiIO *m_io;
        VM &m_vm;
        std::string m_currentError;

        bool m_quitRequested = false;

        // Windows
        bool m_showMainMenu = false;
        bool m_showAbout = false;

        // Popups
        bool m_openErrorPopup = false;
        bool m_showPauseScreen = false;

        Breakpoints   m_breakpoints;
        Disassembler  m_disassembler;
        InstrExecutor m_instrExecutor;
        Keypad        m_keypad;
        Registers     m_registers;
        Settings      m_settings;
        Stack         m_stack;
    };
}
