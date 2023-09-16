// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/ui/keypad.hpp>
#include <nchip8/imgui.hpp>

#include <cstddef>

using namespace nchip8::ui;

Keypad::Keypad(VM &vm)
    : Window { "Keypad", ImGuiWindowFlags_AlwaysAutoResize },
      m_vm   { vm } {
}

void Keypad::body() {
    static std::bitset<KEY_COUNT> states;
    const auto &layout = m_vm.cfg.input.layout;

    for (std::size_t i = 0; i < KEY_COUNT; ++i) {
        const auto &key = layout[i];
        std::size_t keyIdx = (std::size_t) key.second;
        bool pressed = states[keyIdx];

        if (pressed) {
            ImGui::PushStyleColor(ImGuiCol_Button, 0x00);
        }

        if (ImGui::Button(SDL_GetScancodeName(key.first))) {
            states.flip(keyIdx);
            m_vm.state.inputTable[keyIdx] = states[keyIdx];
        }

        if (pressed) {
            ImGui::PopStyleColor();
        }

        // Until it's the fourth button, align the buttons on the same line
        if (!utils::divisibleBy((int) i + 1, 4)) {
            ImGui::SameLine();
        }
    }
}
