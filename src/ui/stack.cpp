// Copyright (c) 2024 inunix3.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/ui/stack.hpp>
#include <nchip8/imgui.hpp>

#include <cinttypes>
#include <cstddef>

using namespace nchip8::ui;

Stack::Stack(VM &vm)
    : Window { "Stack", ImGuiWindowFlags_AlwaysAutoResize },
      m_vm { vm } {
}

void Stack::body() {
    auto &stack = m_vm.state.stack;

    if (ImGui::BeginTable("Stack", 2, ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("No.");
        ImGui::TableSetupColumn("Data");
        ImGui::TableHeadersRow();

        for (std::size_t i = 0; i < stack.size(); ++i) {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%zu", i + 1);

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("0x%.4" PRIx16, stack.c[i]);
        }

        ImGui::EndTable();
    }
}
