// Copyright (c) 2024 inunix3.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#include "window.hpp"
#include "../config.hpp"
#include "../imgui.hpp"
#include "../vm.hpp"

namespace nchip8::ui {
    class UI;
    class Settings : public Window {
    public:
        Settings(sdl::Window &window, VM &vm, UI &ui);

    private:
        void body() override;

        void sectionCPU();
        void sectionQuirks();
        void sectionGraphics();
        void sectionInput();
        void sectionSound();
        void sectionUI();

        void markerNotSaved();
        void marker(const std::string &text);

        sdl::Window &m_window;
        VM     &m_vm;
        UI     &m_ui;
        Config  m_newCfg;
        Quirks  m_quirks;
        ImVec4  m_offColor;
        ImVec4  m_onColor;
        bool    m_enableGrid;
    };
}
