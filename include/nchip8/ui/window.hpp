// Copyright (c) 2024 inunix3.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#include "../imgui.hpp"

#include <string>

namespace nchip8::ui {
    class Window {
    public:
        Window(const std::string &title, ImGuiWindowFlags flags);
        virtual ~Window();

        void render();

        bool show = false;

    protected:
        virtual void body() = 0;

        std::string m_title;
        ImGuiWindowFlags m_flags;
    };
}

