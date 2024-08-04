// Copyright (c) 2024 inunix3.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/ui/registers.hpp>
#include <nchip8/imgui.hpp>

#include <cinttypes>
#include <cstddef>

using namespace nchip8::ui;

Registers::Registers(VM &vm)
    : Window { "Registers", ImGuiWindowFlags_AlwaysAutoResize },
      m_vm   { vm } {
}

void Registers::body() {
    auto &state = m_vm.state;
    auto pc   = m_vm.state.pc;
    auto regI = m_vm.state.i;
    auto dt   = m_vm.state.dt;
    auto st   = m_vm.state.st;
    auto regs = m_vm.state.regs;

    ImGui::PushItemWidth(ImGui::GetFontSize() * 5);

    auto flags = ImGuiInputTextFlags_EnterReturnsTrue;

    if (ImGui::InputScalar("PC", ImGuiDataType_U16, &pc, nullptr, nullptr, "%04" PRIx16, flags)) {
        state.pc = pc;
    }

    if (ImGui::InputScalar("I",  ImGuiDataType_U16, &regI, nullptr, nullptr, "%04" PRIx16, flags)) {
        state.i = regI;
    }

    if (ImGui::InputScalar("DT", ImGuiDataType_U8, &dt, nullptr, nullptr, "%02" PRIx8, flags)) {
        state.dt = dt;
    }

    if (ImGui::InputScalar("ST", ImGuiDataType_U8, &st, nullptr, nullptr, "%02" PRIx8, flags)) {
        state.st = st;
    }

    for (std::size_t i = 0; i < state.regs.size(); ++i) {
        if (ImGui::InputScalar(("V" + utils::toHexUpper<std::size_t, 1>(i)).c_str(), ImGuiDataType_U8, &regs[i],
                nullptr, nullptr, "%02" PRIx8, flags)) {
            state.regs[i] = regs[i];
        }
    }

    ImGui::PopItemWidth();
}
