// Copyright (c) 2024 inunix3.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/ui/window.hpp>

using namespace nchip8::ui;

Window::Window(const std::string &title, ImGuiWindowFlags flags)
    : m_title { title },
      m_flags { flags } {

}

Window::~Window() {

}

void Window::render() {
    if (!show) {
        return;
    }

    if (!ImGui::Begin(m_title.c_str(), &show, m_flags)) {
        ImGui::End();

        return;
    }

    body();

    ImGui::End();
}
